#ifndef PSRAM_H
#define PSRAM_H

#include <cstdint>
#include "../Constants.h"

extern "C" uint8_t external_psram_size; // in MB
namespace mmfs
{

    class PSRAMFile;
    class PSRAM
    {
        inline int min(int a, int b) { return a < b ? a : b; }
        inline int max(int a, int b) { return a > b ? a : b; }

    private:
        bool ready = false;
        int size = external_psram_size * ONE_MB; // in bytes
        uint16_t numClusters = size / PSRAM_CLUSTER_SIZE;
        uint32_t *clusterMap = new uint32_t[numClusters]; // first 16 bits are the next cluster's # (0 indexed), last 16 bits are the file id # (1 indexed); or 0 if free
        uintptr_t *fileMap = new uintptr_t[MAX_PSRAM_FILES];    // index is file id - 1, value is file pointer
        uint16_t getNextFreeCluster(uint16_t currentCluster);

        void write(uintptr_t address, const char *data, int size);

    public:
        PSRAM();
        virtual ~PSRAM();
        bool init();
        bool isReady() const;
        PSRAMFile *open(const char *name, bool create = false);
        PSRAMFile *open(int index); // 1 indexed, faster than name if you know the index

        void writeFile(PSRAMFile &file, const char *data, int size);
        char *readNextFileCluster(PSRAMFile &file, int &size); // returns a pointer to actual memory, without copying to a buffer
        void readFile(PSRAMFile &file, char *data, int size);

        int getSize() { return size; }
    };

} // namespace mmfs

extern mmfs::PSRAM *psram;

#endif // PSRAM_H