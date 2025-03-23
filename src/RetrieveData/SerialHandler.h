#include <Arduino.h>
#include <SdFat.h>
#include "../RecordData/Logger.h"
namespace mmfs
{
    class SerialHandler
    {
        enum Commands{
            LS,
            CP,
            CLR,
            LATEST,
            RM
        };
        public:
            void handle();
        private:
            void fetchList();
            void clearFiles();
            void removeFile(char *args);
            void latestFiles();
            void copyFile(char *args);
        private:
            char cmdBuffer[250];
    };
}