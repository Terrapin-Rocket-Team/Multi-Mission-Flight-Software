#ifndef PSRAMFILE_H
#define PSRAMFILE_H

#include <cstdint>
#include "../Constants.h"
#include <stdio.h>

namespace mmfs
{
    inline const uint8_t F_READ = 0b001;
    inline const uint8_t F_WRITE = 0b010;
    inline const uint8_t F_APPEND = 0b100;

    inline const uint8_t F_SEEK_SET = 1;
    // inline const uint8_t F_SEEK_CUR = 2; // not implemented
    inline const uint8_t F_SEEK_END = 3;
    class PSRAMFile
    {

        friend class PSRAM;

    private:
        uintptr_t endOfFile = 0;                   // address of the last byte written
        uintptr_t cursor = 0;                      // address of current location, offset by PSRAM_STARTING_ADDRESS
        uint8_t clusterCursor = 0;                 // similar to cursor, but used for reading one cluster at a time, index of the current cluster
        char *name = new char[MAX_FILE_NAME_SIZE]; // file name
        uint16_t id;                               // The file's ID # according to the PSRAM (order of creation)
        int size = 0;                              // in bytes
        uint8_t startCluster;                      // first cluster
        uint8_t eofCluster;                        // EOF cluster
        bool _open = false;                        // whether the file is open

        uint8_t openMode = F_READ;

    public:
        PSRAMFile(const char *name);
        ~PSRAMFile();

        void write(uint8_t *data, int size);
        void print(const char *data);
        void println(const char *data);

        int read(char *data, int size);
        void readTo(char *data, char endChar);

        void setName(const char *name);
        const char *getName();
        int getSize();

        void restart();

        void seek(int offset, uint8_t origin = F_SEEK_SET);

        void toEnd();

        void close();

        bool isOpen() const;

        void open(uint8_t mode);

        int getCursor() const;
    };

} // namespace mmfs

#endif // PSRAMFILE_H