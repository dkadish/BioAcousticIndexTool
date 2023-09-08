FROM python:3.9.0-slim

ENV APP_VERSION="6.1.11" \
    APP="platformio-core"

LABEL app.name="${APP}" \
      app.version="${APP_VERSION}" \
      maintainer="David Kadish <david.kadish@gmail.com>"

RUN pip install -U platformio==${APP_VERSION} && \
    mkdir -p /code && \
    mkdir -p /.platformio && \
    chmod a+rwx /.platformio && \
    apt update && apt install -y git && apt-get clean autoclean && apt-get autoremove --yes && rm -rf /var/lib/{apt,dpkg,cache,log}/

#USER 1001
COPY platformio.ini /code/platformio.ini
COPY include/ /code/include/
COPY src/ /code/src/
#COPY lib/ /code/lib/
COPY lib/Display/ /code/lib/Display/
#COPY lib/Ecoacoustics/ /code/lib/Ecoacoustics/
COPY lib/Environment /code/lib/Environment
COPY lib/Logging /code/lib/Logging
COPY lib/Sensor /code/lib/Sensor

WORKDIR /code

CMD ["platformio", "run", "-v"]