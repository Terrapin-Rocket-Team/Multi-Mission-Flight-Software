#include "NativeTestHelper.h"

extern "C" {
    uint8_t external_psram_size = 1;
}
FsFileData *files[10] = {nullptr};
