#include "psram.h"
#include "PSRAMFile.h"
#include <string.h>
#include <stdio.h>

using namespace mmfs;

PSRAM::PSRAM()
{
    for (int i = 0; i < numClusters; i++)
    {
        clusterMap[i] = 0;
    }
    for (int i = 0; i < MAX_PSRAM_FILES; i++)
    {
        fileMap[i] = 0;
    }
}

PSRAM::~PSRAM()
{
}

bool PSRAM::init()
{
    if (external_psram_size == 0)
    {
        return false;
    }
    return ready = true;
}

bool PSRAM::isReady() const
{
    return ready;
}

void PSRAM::writeFile(PSRAMFile &file, const char *data, int size)
{
    while (size > 0)
    {
        int clusterOffset = file.endOfFile % PSRAM_CLUSTER_SIZE;
        int spaceInCluster = PSRAM_CLUSTER_SIZE - clusterOffset;

        // Write as much as will fit in the current cluster
        int bytesToWrite = min(spaceInCluster, size);
        write(file.currentCluster * PSRAM_CLUSTER_SIZE + clusterOffset, data, bytesToWrite);

        // Update file position and remaining data
        file.endOfFile += bytesToWrite;
        data += bytesToWrite;
        size -= bytesToWrite;
        file.size += bytesToWrite;

        // If the current cluster is full, move to the next one
        if (file.endOfFile % PSRAM_CLUSTER_SIZE == 0)
        {
            uint16_t nextCluster = getNextFreeCluster(file.currentCluster);

            if (nextCluster == (uint16_t)-1) // No more free clusters (out of space)
                return;            // TODO: Handle this case
            else{
                uint16_t ttt = nextCluster;
                uint16_t ttt2 = (uint16_t)-1;
                printf("Next cluster: %hd\n", nextCluster);
                printf("-1: %hd\n", (uint16_t)-1);
            }

            clusterMap[file.currentCluster] = nextCluster << 16 | file.id;
            file.currentCluster = nextCluster;
            clusterMap[file.currentCluster] = (0xFFFF << 16) | file.id; // there is no next cluster
        }
    }
}

void PSRAM::write(uintptr_t address, const char *data, int size)
{
    // Write data to PSRAM
    for (int i = 0; i < size; i++)
        *((char *)(PSRAM_STARTING_ADDRESS + address + i)) = data[i];
}

uint16_t PSRAM::getNextFreeCluster(uint16_t currentCluster)
{
    for (int i = 0; i < numClusters; i++)
    {                                                         // Loop through numClusters steps
        int nextCluster = (currentCluster + i) % numClusters; // Wrap around using modulo
        if (clusterMap[nextCluster] == 0)
            return nextCluster;
    }
    return -1; // No free clusters (0xFFFF in uint16_t)a
}

PSRAMFile *PSRAM::open(const char *name, bool create)
{
    // Find the file in the fileMap
    for (int i = 0; i < MAX_PSRAM_FILES; i++)
    {
        if (fileMap[i] != 0)
        {
            PSRAMFile *file = (PSRAMFile *)fileMap[i];
            if (strcmp(file->getName(), name) == 0)
                return file;
        }
        else if (create && getNextFreeCluster(0) != (uint16_t)-1) // Create a new file if it doesn't exist
        {
            PSRAMFile *file = new PSRAMFile(name);
            file->id = i + 1;
            file->startCluster = getNextFreeCluster(0);
            file->currentCluster = file->startCluster;
            file->clusterCursor = file->startCluster;
            fileMap[i] = (uintptr_t)file;
            clusterMap[file->startCluster] = (0xFFFF << 16) | file->id; // there is no next cluster yet
            return file;
        }
        else
            return nullptr; // File not found
    }
    return nullptr; // No free file slots
}

PSRAMFile *PSRAM::open(int index)
{
    if (index < 1 || index > MAX_PSRAM_FILES)
        return nullptr;

    if (fileMap[index - 1] != 0)
        return (PSRAMFile *)fileMap[index - 1];

    return nullptr;
}

char *PSRAM::readNextFileCluster(PSRAMFile &file, int &size)
{
    if(file.clusterCursor == 0xFFFF){
        size = 0;
        return nullptr;
    }
    if (file.currentCluster == file.clusterCursor)
        size = file.endOfFile % PSRAM_CLUSTER_SIZE;
    else
        size = PSRAM_CLUSTER_SIZE;
    uintptr_t curCluster = file.clusterCursor;

    if (file.clusterCursor == file.currentCluster) // If we're at the end of the file, start over
        file.clusterCursor = 0xFFFF;
    else
        file.clusterCursor = clusterMap[file.clusterCursor] >> 16;

    return (char *)(PSRAM_STARTING_ADDRESS + curCluster * PSRAM_CLUSTER_SIZE);
}

void PSRAM::readFile(PSRAMFile &file, char *data, int size)
{
    uintptr_t start = PSRAM_STARTING_ADDRESS + file.startCluster * PSRAM_CLUSTER_SIZE;
    uint16_t nextCluster = clusterMap[file.startCluster] >> 16;
    int i = 0;
    for (i; i < size && i < file.size; i++)
    {
        data[i] = *((char *)(start + i % PSRAM_CLUSTER_SIZE));
        if ((i+1) % PSRAM_CLUSTER_SIZE == 0)
        {
            if (nextCluster == 0xFFFF)
                break; // No more clusters for this file)
            start = PSRAM_STARTING_ADDRESS + nextCluster * PSRAM_CLUSTER_SIZE;
            nextCluster = clusterMap[nextCluster] >> 16;
        }
    }
    if (i < size - 1)
        data[i] = '\0';
    else
        data[size - 1] = '\0';
}