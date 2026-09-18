#!/usr/bin/env python3
"""TTN -> InfluxDB bridge.

Data path:  Teensy --LoRaWAN--> TTN --MQTT--> this bridge --> InfluxDB
"""
import numbers
import os
import sys
import json
import traceback

import paho.mqtt.client as mqtt
from influxdb_client import InfluxDBClient, Point
from influxdb_client.client.write_api import SYNCHRONOUS


def flatten(obj, prefix=""):
    """Flatten nested dicts/lists into {dotted_underscore_key: scalar}.

    Matches Telegraf's json parser convention: nested objects join with "_",
    list elements get their index as a path segment
    (e.g. uplink_message_rx_metadata_0_rssi).
    """
    out = {}
    if isinstance(obj, dict):
        for k, v in obj.items():
            out.update(flatten(v, f"{prefix}_{k}" if prefix else str(k)))
    elif isinstance(obj, (list, tuple)):
        for i, v in enumerate(obj):
            out.update(flatten(v, f"{prefix}_{i}" if prefix else str(i)))
    else:
        out[prefix] = obj
    return out


def _env(name, default=None, required=False):
    val = os.environ.get(name, default)
    if required and not val:
        sys.exit(f"FATAL: required environment variable {name} is not set")
    return val


TTN_REGION = _env("TTN_REGION", "eu1")
TTN_TENANT = _env("TTN_TENANT", "ttn")
TTN_APP_ID = _env("TTN_APP_ID", required=True)
TTN_API_KEY = _env("TTN_API_KEY", required=True)
# Restrict to a single device id (as shown in the TTN console). Empty = all devices.
TTN_DEVICE_ID = _env("TTN_DEVICE_ID", "")
TTN_HOST = _env("TTN_MQTT_HOST", f"{TTN_REGION}.cloud.thethings.network")
TTN_PORT = int(_env("TTN_MQTT_PORT", "8883"))
# TTN MQTT username is "<app-id>@<tenant>"; password is the API key.
TTN_USERNAME = _env("TTN_USERNAME", f"{TTN_APP_ID}@{TTN_TENANT}")

INFLUX_URL = _env("INFLUX_URL", "http://influxdb:8086")
INFLUX_TOKEN = _env("INFLUX_TOKEN", required=True)
INFLUX_ORG = _env("INFLUX_ORG", "bait")
INFLUX_BUCKET = _env("INFLUX_BUCKET", "bait2")

print(f"Bridge starting: TTN {TTN_USERNAME}@{TTN_HOST}:{TTN_PORT} "
      f"device={TTN_DEVICE_ID or '(all)'} "
      f"-> InfluxDB {INFLUX_URL} (org={INFLUX_ORG}, bucket={INFLUX_BUCKET})", flush=True)

try:
    influx_client = InfluxDBClient(url=INFLUX_URL, token=INFLUX_TOKEN, org=INFLUX_ORG)
    write_api = influx_client.write_api(write_options=SYNCHRONOUS)
    influx_client.ready()
    print("InfluxDB connection OK", flush=True)
except Exception as exc:  # noqa: BLE001
    print(f"InfluxDB connection failed: {exc}", flush=True)
    traceback.print_exc()
    sys.exit(1)


def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code != 0:
        print(f"TTN MQTT connect failed: {reason_code}", flush=True)
        return
    # Subscribe to every device's uplink topic; writes are still filtered to
    # TTN_DEVICE_ID below. This makes it visible when uplinks are arriving for
    # other device ids (i.e. the id is wrong) vs not arriving at all.
    topic = f"v3/{TTN_USERNAME}/devices/+/up"
    print(f"Connected to TTN MQTT, subscribing to {topic} (qos=1)", flush=True)
    client.subscribe(topic, qos=1)


def on_subscribe(client, userdata, mid, reason_codes, properties):
    print(f"Subscription active: {[str(rc) for rc in reason_codes]}", flush=True)


def on_disconnect(client, userdata, flags, reason_code, properties):
    print(f"TTN MQTT disconnected ({reason_code}); paho will auto-reconnect", flush=True)


def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        uplink = payload.get("uplink_message", {})
        decoded = uplink.get("decoded_payload") or {}
        device_id = payload["end_device_ids"]["device_id"]
        received_at = uplink.get("received_at") or payload.get("received_at")

        if TTN_DEVICE_ID and device_id != TTN_DEVICE_ID:
            print(f"uplink from '{device_id}' (ignored; want '{TTN_DEVICE_ID}')", flush=True)
            return

        # One point per uplink, with the whole uplink_message JSON flattened onto
        point = Point("sensor_data").tag("device", device_id).tag("source", "ttn")
        point = point.field("topic", msg.topic)

        n_fields = 0
        for key, value in flatten(uplink, "uplink_message").items():
            if isinstance(value, bool):
                point = point.field(key, int(value))
            elif isinstance(value, numbers.Number):
                point = point.field(key, float(value))
            elif isinstance(value, str) and value != "":
                point = point.field(key, value)
            else:
                continue
            n_fields += 1

        if received_at:
            point = point.time(received_at)

        write_api.write(bucket=INFLUX_BUCKET, org=INFLUX_ORG, record=point)
        note = "" if decoded else " (no decoded_payload - TTN payload formatter not set?)"
        print(f"[{received_at}] {device_id}: wrote {n_fields} fields{note}", flush=True)
    except Exception as exc:  # noqa: BLE001
        print(f"on_message error: {exc}", flush=True)
        traceback.print_exc()


def main():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.username_pw_set(TTN_USERNAME, TTN_API_KEY)
    client.tls_set()
    client.on_connect = on_connect
    client.on_subscribe = on_subscribe
    client.on_disconnect = on_disconnect
    client.on_message = on_message

    print("Connecting to TTN MQTT...", flush=True)
    client.connect(TTN_HOST, TTN_PORT, keepalive=60)
    client.loop_forever(retry_first_connection=True)


if __name__ == "__main__":
    main()
