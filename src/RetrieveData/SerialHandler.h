#include <Arduino.h>
#include "../RecordData/Logging/Logger.h"

#define LS_DATE 2
#define LS_SIZE 4

namespace mmfs

{
    class SerialHandler
    {
        public:
            void handle();
            const char *getLastLine() const;
        private:
            void fetchList();
            void clearFiles();
            void removeFile(char *args);
            void latestFiles();
            void copyFile(char *args);
        private:
            char line[2500];
    };
    SerialHandler &getSerialHandler();
}