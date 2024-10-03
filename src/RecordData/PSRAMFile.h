#ifndef PSRAMFILE_H
#define PSRAMFILE_H

#include <cstdint>
#include "../Constants.h"
#include "StorageFile.h"

namespace mmfs
{
    class PSRAMFile
    {

        friend class PSRAM;

    private:
        uintptr_t endOfFile = 0;
        uintptr_t clusterCursor = 0; // in clusters
        char *name = new char[MAX_FILE_NAME_SIZE];
        uint16_t id;
        int size = 0;
        uint16_t startCluster;
        uint16_t currentCluster;

    public:
        PSRAMFile(const char *name);

        void write(uint8_t *data, int size);
        void print(const char *data);
        void println(const char *data);

        void read(char *data, int size);
        void readTo(char *data, char endChar);

        void setName(const char *name);
        const char *getName();
        int getSize();

        void restart();
    };

} // namespace mmfs

#endif // PSRAMFILE_H