#include "psram.h"
#include "PSRAMFile.h"
#include <string.h>
#include <stdio.h>

using namespace mmfs;

PSRAM::PSRAM()
{
    for (int i = 0; i < numClusters; i++)
        clusterMap[i] = 0;
    for (int i = 0; i < MAX_PSRAM_FILES; i++)
        fileMap[i] = 0;
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
    {                                                            // cluster being written to in this cycle of the loop
        int clusterOffset = file.cursor % PSRAM_CLUSTER_SIZE;    // how far into the cluster the cursor is (in bytes)
        int spaceInCluster = PSRAM_CLUSTER_SIZE - clusterOffset; // how much space is left in the cluster (in bytes)

        // Write as much as will fit in the current cluster
        int bytesToWrite = min(spaceInCluster, size);                                       // how many bytes will be written in this cycle of the loop
        write(file.clusterCursor * PSRAM_CLUSTER_SIZE + clusterOffset, data, bytesToWrite); // write the data to the PSRAM

        // Update file position and remaining data
        data += bytesToWrite;
        size -= bytesToWrite;

        file.cursor += bytesToWrite;
        if (file.clusterCursor == file.eofCluster) // if we're writing to the cluster containing the EOF
        {
            if (file.cursor > file.endOfFile)
            {
                file.size += file.cursor - file.endOfFile;
                file.endOfFile = file.cursor;
            }
        }

        // If the current cluster is full, move to the next one
        if (file.cursor % PSRAM_CLUSTER_SIZE == 0)
        {
            uint8_t nextCluster = getNextFreeCluster(file.eofCluster);

            if (nextCluster == (uint8_t)-1) // No more free clusters (out of space)
                return;                     // TODO: Handle this case

            // Change this cluster's next cluster to the new one, preserving the previous cluster
            clusterMap[file.eofCluster] = (clusterMap[file.eofCluster] & 0xFF000000) | (nextCluster << 16) | file.id;
            // set the next cluster's previous cluster to the current cluster
            clusterMap[nextCluster] = (file.eofCluster << 24) | (0xFF << 16) | file.id; // there is no next cluster
            if (size > 0)
            {
                // set eof to the address of nextCluster so it doesnt get left behind in this one as we continue writing
                file.endOfFile = nextCluster * PSRAM_CLUSTER_SIZE;
                file.clusterCursor = nextCluster;
                file.eofCluster = nextCluster;
                file.cursor = nextCluster * PSRAM_CLUSTER_SIZE;
            }
        }
    }
}

void PSRAM::write(uintptr_t address, const char *data, int size)
{
    // Write data to PSRAM
    for (int i = 0; i < size; i++)
        *((char *)(PSRAM_STARTING_ADDRESS + address + i)) = data[i];
}

uint8_t PSRAM::getNextFreeCluster(uint8_t currentCluster)
{
    for (int i = 0; i < numClusters; i++)
    {                                                         // Loop through numClusters steps
        int nextCluster = (currentCluster + i) % numClusters; // Wrap around using modulo
        if (clusterMap[nextCluster] == 0)
            return nextCluster;
    }
    return -1; // No free clusters (0xFF in uint8_t)
}

PSRAMFile *PSRAM::open(const char *name, uint8_t mode, bool create)
{
    // Find the file in the fileMap
    for (int i = 0; i < MAX_PSRAM_FILES; i++)
    {
        if (fileMap[i] != 0)
        {
            PSRAMFile *file = (PSRAMFile *)fileMap[i];
            if (strcmp(file->getName(), name) == 0)
            {
                file->open(mode);
                return file;
            }
        }
        else if (create && getNextFreeCluster(0) != (uint8_t)-1) // Create a new file if it doesn't exist
        {
            PSRAMFile *file = new PSRAMFile(name, this);
            file->id = i;
            file->startCluster = getNextFreeCluster(0);
            file->eofCluster = file->startCluster;
            file->clusterCursor = file->startCluster;
            file->cursor = file->startCluster * PSRAM_CLUSTER_SIZE;
            file->endOfFile = file->cursor;
            fileMap[i] = (uintptr_t)file;
            clusterMap[file->startCluster] = (0xFFFF << 16) | file->id; // there is no next or pervious cluster yet
            file->open(mode);
            return file;
        }
        else
            return nullptr; // File not found
    }
    return nullptr; // No free file slots
}

PSRAMFile *PSRAM::open(int index, uint8_t mode)
{
    if (index < 0 || index >= MAX_PSRAM_FILES)
        return nullptr;

    if (fileMap[index] != 0)
    {
        PSRAMFile *file = (PSRAMFile *)fileMap[index];
        file->open(mode);
        return file;
    }

    return nullptr;
}

char *PSRAM::readNextFileCluster(PSRAMFile &file, int &size)
{
    if (file.clusterCursor == 0xFF)
    {
        size = 0;
        return nullptr;
    }
    if (file.eofCluster == file.clusterCursor)
        size = file.endOfFile % PSRAM_CLUSTER_SIZE;
    else
        size = PSRAM_CLUSTER_SIZE;
    uint8_t curCluster = file.clusterCursor;

    if (file.clusterCursor == file.eofCluster) // If we're at the end of the file, indicate it
        file.clusterCursor = 0xFF;
    else
        file.clusterCursor = clusterMap[file.clusterCursor] >> 16;

    return (char *)(PSRAM_STARTING_ADDRESS + curCluster * PSRAM_CLUSTER_SIZE);
}

int PSRAM::readFile(PSRAMFile &file, char *data, int size)
{
    uintptr_t start = PSRAM_STARTING_ADDRESS + file.cursor;
    uint8_t nextCluster = clusterMap[file.clusterCursor] >> 16;
    int i = 0;

    for (; i < size && !(file.clusterCursor == file.eofCluster && file.cursor >= file.endOfFile); i++)
    {
        data[i] = *((char *)(start + (i % PSRAM_CLUSTER_SIZE)));
        file.cursor++;
        if ((file.cursor) % PSRAM_CLUSTER_SIZE == 0)
        {
            if (nextCluster == 0xFF)
                break; // No more clusters for this file
            start = PSRAM_STARTING_ADDRESS + nextCluster * PSRAM_CLUSTER_SIZE;
            file.clusterCursor = nextCluster;
            file.cursor = nextCluster * PSRAM_CLUSTER_SIZE;
            nextCluster = clusterMap[nextCluster] >> 16;
        }
    }
    return i;
}

void PSRAM::seekFile(PSRAMFile &file, int offset, uint8_t origin)
{
    switch (origin)
    {
    case F_SEEK_SET:
    {
        offset = min(file.size, max(0, offset));
        int numClusters = offset / PSRAM_CLUSTER_SIZE;
        file.clusterCursor = file.startCluster;
        for (int i = 0; i < numClusters; i++)
            file.clusterCursor = clusterMap[file.clusterCursor] >> 16;
        file.cursor = file.clusterCursor * PSRAM_CLUSTER_SIZE + offset % PSRAM_CLUSTER_SIZE;
        break;
    }
    // case F_SEEK_CUR:
    // {
    //     break;
    // }
    case F_SEEK_END:
        seekFile(file, file.size - offset, F_SEEK_SET);
        break;
    }
}