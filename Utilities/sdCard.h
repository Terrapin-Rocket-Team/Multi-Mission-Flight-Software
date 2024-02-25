#ifndef SD_CARD_H
#define SD_CARD_H

#include "SdFat.h"

#define COMPACT_WALKBACK_COUNT 50

extern SdFs sd;
extern FsFile dataFile;
extern String dataFileName;
extern FsFile logFile;
extern String logFileName;

bool setupSDCard(String csvHeader);  // Initializes the sensor
bool isSDReady();  // Returns whether the sensor is initialized

template< typename T > void sdPrintln( T data, String fileName ){
  String str = String(data);
  logFile = sd.open(fileName, FILE_WRITE);
  if (logFile) {
    logFile.println(str);
    logFile.close();
  }
}

template< typename T > void sdPrint( T data, String fileName ){
  String str = String(data);
  logFile = sd.open(fileName, FILE_WRITE);
  if (logFile) {
    logFile.print(str);
    logFile.close();
  }
}


#endif