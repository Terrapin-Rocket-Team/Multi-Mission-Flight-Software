#include <Arduino.h>
#include <SdFat.h>
#include "../RecordData/Logging/Logger.h"
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