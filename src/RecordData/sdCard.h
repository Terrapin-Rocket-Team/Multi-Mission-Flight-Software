#ifndef SD_CARD_H
#define SD_CARD_H

#ifdef ARDUINO
#include <Arduino.h>
#include <SdFat.h>
#else
#include "../../test/NativeTestMocks/SdFat.h"
#include "../../test/NativeTestMocks/Arduino.h"
#endif

extern SdFs sd;
extern FsFile logFile;
extern FsFile flightDataFile;
extern char logFileName[];
extern char flightDataFileName[];

bool setupSDCard(); // Initializes the card
bool isSDReady();  // Returns whether the sensor is initialized
void sendSDCardHeader(const char *csvHeader); // Sends the header to the SD card

#endif