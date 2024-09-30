/**
 * @file Logger.h
 * @brief This file contains the declaration of the Logger class, an example use case of the PSRAM and SdCardFile classes.
 * Based on the mode, the Logger class will write data to the PSRAM or the SD card, writing to the SD card when the mode is GROUND
 * and writing to the PSRAM when the mode is FLIGHT. The Logger class will also write data to the USB serial port when connected if
 * the destination is set to TO_USB or BOTH, which it is by default.
*/

#ifndef LOGGER_H
#define LOGGER_H

#include "psram.h"
#include "SdCardStorage.h"
#include "../Constants.h"


namespace mmfs {

enum LogType {
    LOG_,
    ERROR_,
    WARNING_,
    INFO_
};

enum Dest {
    TO_FILE,
    TO_USB,
    BOTH
};

enum Mode {
    GROUND,
    FLIGHT
};

enum GroundDest {
    BUFFER,
    ALTERNATING_BOTH
};

class Logger {
private:
    PSRAM ram;
    SdCardStorage sdCard;
    bool SDready;
    bool RAMready;
    bool dumped;
    GroundDest groundDest;
    /** The buffer size of the circular buffer, in bytes, for preflight */
    uint16_t bufferSize;
    /** If groundDest is ALTERNATING_BOTH, this is the number of data entries to be put to the PSRAM between each write to the file */
    unsigned int bufferInterval;
    uint32_t bufIdx;
    uint32_t bufCount;
    Mode mode;

public:

    Logger(GroundDest groundDest = ALTERNATING_BOTH, uint16_t bufferSize = 25000, int bufferInterval = 300);

    void init();

    bool isPsramReady() const { return RAMready; }
    bool isSdCardReady() const { return SDready; }
    bool isReady() const { return RAMready && SDready; }

    /** Takes the pointer to a null terminated character array and prints it to the appropriate destination based on the mode */
    void recordFlightData(char *data); 
    /** 
     * Takes the type of log, a pointer to a null terminated character array, and the destination and prints it to the appropriate destination based on the mode.
     * This data is treated as a log, and thus will be written to the bottom of the PSRAM if the mode is FLIGHT, and to the SD card if the mode is GROUND.
     */
    void recordLogData(LogType type, const char *data, Dest dest = BOTH);
    /** 
     * Takes the timestamp, the type of log, a pointer to a null terminated character array, and the destination and prints it to the appropriate destination based on the mode.
     * This data is treated as a log, and thus will be written to the bottom of the PSRAM if the mode is FLIGHT, and to the SD card if the mode is GROUND.
     */
    void recordLogData(double timeStamp, LogType type, const char *data, Dest dest = BOTH);
    /** 
     * Takes the mode and enables or disables the PSRAM based on the mode. If mode is GROUND, PSRAM will be disabled and all data in PSRAM will be written to the SD card. 
     * If mode is FLIGHT, PSRAM will be enabled. Switching the mode from FLIGHT to GROUND will cause all data in PSRAM to be dumped to the SD card, creating a data and log file.
     */
    void setRecordMode(Mode mode);
    /**
     * Dumps existing data from the PSRAM to the data file and log file on the SD card.
     */
    void dumpData();
    

};


} // namespace mmfs

extern mmfs::Logger logger;

#endif //LOGGER_H