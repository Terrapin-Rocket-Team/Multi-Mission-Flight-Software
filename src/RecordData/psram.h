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

        friend class PSRAMFile;

        inline int min(int a, int b) { return a < b ? a : b; }
        inline int max(int a, int b) { return a > b ? a : b; }

    private:
        bool ready = false;
        int size = external_psram_size * ONE_MB; // in bytes
        uint8_t numClusters = size / PSRAM_CLUSTER_SIZE; //TODO: Allow more clusters
        uint32_t *clusterMap = new uint32_t[numClusters]; // first 8 bits are the previous cluster's # (0 indexed), then 8 bits the next cluster, then 8 bits for the file id, then 8 bits are reserved (0 if cluster is free)
        uintptr_t *fileMap = new uintptr_t[MAX_PSRAM_FILES];    // index is file id, value is file pointer
        uint8_t getNextFreeCluster(uint8_t currentCluster);

        void write(uintptr_t address, const char *data, int size);

        void seekFile(PSRAMFile &file, int offset, uint8_t origin);
        int readFile(PSRAMFile &file, char *data, int size);
        void writeFile(PSRAMFile &file, const char *data, int size);

    public:
        PSRAM();
        virtual ~PSRAM();
        bool init();
        bool isReady() const;
        PSRAMFile *open(const char *name, uint8_t mode, bool create = false);
        PSRAMFile *open(int index, uint8_t mode); // 1 indexed, faster than name if you know the index

        char *readNextFileCluster(PSRAMFile &file, int &size); // returns a pointer to actual memory, without copying to a buffer

        int getSize() { return size; }
    };

} // namespace mmfs

#endif // PSRAM_H