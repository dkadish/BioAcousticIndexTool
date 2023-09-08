//
// Created by David Kadish on 23/05/2023.
//

#ifndef BAIT2_BAITCONFIG_H
#define BAIT2_BAITCONFIG_H

// Debug definitions
#define VERBOSITY 4
#define USE_STATUS_LED

#ifdef USE_STATUS_LED
#define LEDHIGH digitalWrite(LED_BUILTIN, HIGH);
#define LEDLOW digitalWrite(LED_BUILTIN, LOW);
#else
#define LEDHIGH
#define LEDLOW
#endif

#endif //BAIT2_BAITCONFIG_H
