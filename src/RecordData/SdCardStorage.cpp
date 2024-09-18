#include "SdCardStorage.h"

using namespace mmfs;

static constexpr int NAME_SIZE = 24;
// SdFs sd;
// FsFile logFile;  // File object to use for logging
char logFileName[NAME_SIZE];        // Name of the log file
char flightDataFileName[NAME_SIZE]; // Name of the flight data file
bool sdReady = false;               // Whether the SD card has been initialized

// SD_FAT_TYPE = 0 for SdFat/File as defined in SdFatConfig.h,
// 1 for FAT16/FAT32, 2 for exFAT, 3 for FAT16/FAT32 and exFAT.
#define SD_FAT_TYPE 3
/*
  Change the value of SD_CS_PIN if you are using SPI and
  your hardware does not use the default value, SS.
  Common values are:
  Arduino Ethernet shield: pin 4
  Sparkfun SD shield: pin 8
  Adafruit SD shields and modules: pin 10
*/

// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = SS;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#endif // SDCARD_SS_PIN

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(50)

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif // HAS_SDIO_CLASS

#if SD_FAT_TYPE == 0
SdFat sd;
File logFile;
File flightDataFile;
#elif SD_FAT_TYPE == 1
SdFat32 sd;
File32 logFile;
File32 flightDataFile;
#elif SD_FAT_TYPE == 2
SdExFat sd;
ExFile logFile;
ExFile flightDataFile;
#elif SD_FAT_TYPE == 3
SdFs sd;
FsFile logFile;
FsFile flightDataFile;
#else // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif // SD_FAT_TYPE


SdCardStorage::SdCardStorage() {
    head = nullptr;
    currentFileNode = nullptr;
    sdReady = false;
    fileIndex = 0;
}

SdCardStorage::~SdCardStorage() {
    FileNode* current = head;
    while (current != nullptr) {
        FileNode* next = current->next;
        delete current;
        current = next;
    }
}

bool SdCardStorage::init() {
    if (sd.begin(SD_CONFIG) || sd.restart()) {
        sdReady = true;
    }
    return sdReady;
}

bool SdCardStorage::init(const char *fileSuffix) {
    int rdy = 0;
    sdReady = false;
    if (sd.begin(SD_CONFIG) || sd.restart()) {
        int fileNo = 0;
        bool exists = true;
        char fileName[NAME_SIZE];
        while (exists) {
            snprintf(fileName, NAME_SIZE, "%d_%s", ++fileNo, fileSuffix);
            exists = sd.exists(fileName);
        }
        if (addFile(fileName)) {
            rdy++;
        }
    }
    sdReady = rdy == 1;
    return sdReady;
}

bool SdCardStorage::isReady() const {
    return sdReady;
}

void SdCardStorage::write(char *data, int size) {
    if (currentFileNode && currentFileNode->file.open(currentFileNode->fileName, FILE_WRITE)) {
        currentFileNode->file.seek(currentFileNode->offset);
        currentFileNode->file.write(data, size);
        currentFileNode->offset += size;
        currentFileNode->file.close();
    }
}

void SdCardStorage::print(const char *data) {
    if (currentFileNode && currentFileNode->file.open(currentFileNode->fileName, FILE_WRITE)) {
        currentFileNode->file.seek(currentFileNode->offset);
        currentFileNode->file.print(data);
        currentFileNode->offset += strlen(data);
        currentFileNode->file.close();
    }
}

void SdCardStorage::println(const char *data) {
    if (currentFileNode && currentFileNode->file.open(currentFileNode->fileName, FILE_WRITE)) {
        currentFileNode->file.seek(currentFileNode->offset);
        currentFileNode->file.println(data);
        currentFileNode->offset += strlen(data) + 1; // Include newline character
        currentFileNode->file.close();
    }
}

int SdCardStorage::read(char *data, int size) {
    int bytesRead = -1;
    if (currentFileNode && currentFileNode->file.open(currentFileNode->fileName, FILE_READ)) {
        currentFileNode->file.seek(currentFileNode->offset);
        bytesRead = currentFileNode->file.read(data, size);
        currentFileNode->offset += bytesRead;
        currentFileNode->file.close();
    }
    return bytesRead;
}

int SdCardStorage::readTo(char *data, char endChar) {
    int bytesRead = -1;
    if (currentFileNode && currentFileNode->file.open(currentFileNode->fileName, FILE_READ)) {
        currentFileNode->file.seek(currentFileNode->offset);
        bytesRead = 0;
        while (currentFileNode->file.available() && currentFileNode->file.peek() != endChar) {
            *data = currentFileNode->file.read();
            data++;
            bytesRead++;
        }
        currentFileNode->offset += bytesRead;
        currentFileNode->file.close();
    }
    return bytesRead;
}

bool SdCardStorage::seek(uint64_t offset) {
    if (currentFileNode) {
        currentFileNode->offset = offset;
        return true;
    }
    return false;
}

bool SdCardStorage::addFile(const char* fileName) {
    FileNode* newNode = new FileNode();
    if (newNode == nullptr) {
        return false; // Memory allocation failed
    }
    strncpy(newNode->fileName, fileName, NAME_SIZE);
    newNode->offset = 0;
    newNode->index = ++fileIndex;
    newNode->next = head;
    head = newNode;
    return true;
}

bool SdCardStorage::selectFile(const char* fileName) {
    FileNode* current = head;
    while (current != nullptr) {
        if (strcmp(current->fileName, fileName) == 0) {
            currentFileNode = current;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool SdCardStorage::selectFile(int index) {
    FileNode* current = head;
    while (current != nullptr) {
        if (current->index == index) {
            currentFileNode = current;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool SdCardStorage::selectNextFile() {
    if (currentFileNode) {
        if (currentFileNode->next) {
            currentFileNode = currentFileNode->next;
        } else {
            currentFileNode = head;
        }
        return true;
    }
    return false;
}
