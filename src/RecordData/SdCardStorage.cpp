#include "SdCardStorage.h"

using namespace mmfs;

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
