//
// Created by David Kadish on 25/05/2023.
//

#ifndef BAIT2_LOGGING_H
#define BAIT2_LOGGING_H

/* Debug messages */
#ifndef VERBOSITY
    #define VERBOSITY 4
#endif

#if VERBOSITY >= 1
#define ERROR(msg, ...) Serial.printf(msg, ##__VA_ARGS__); Serial.println();
#else
#define ERROR(...)
#endif

#if VERBOSITY >= 2
#define WARNING(msg, ...) Serial.printf(msg, ##__VA_ARGS__); Serial.println();
#else
#define WARNING(...)
#endif

#if VERBOSITY >= 3
#define INFO(msg, ...) Serial.printf(msg, ##__VA_ARGS__); Serial.println();
#else
#define INFO(...)
#endif

#if VERBOSITY >= 4
#define DEBUG(msg, ...) Serial.printf(msg, ##__VA_ARGS__); Serial.println();
#else
#define DEBUG(...)
#endif

#if VERBOSITY >= 5
#define TRACE(msg, ...) Serial.printf(msg, ##__VA_ARGS__); Serial.println();
#else
#define TRACE(...)
#endif

#endif //BAIT2_LOGGING_H
