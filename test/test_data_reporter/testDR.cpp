#include "../../src/RecordData/DataReporter.h"

namespace mmfs
{
    class testDR : public DataReporter
    {
    public:
        PackedInfo *first = nullptr, *last = nullptr;
        int packedDataSize = 0;
        uint8_t numColumns = 0;
        uint8_t *packedData = nullptr;
        int var1 = 0;
        double var2 = 0;
        testDR(){
            addColumn(INT, &var1, "int");
            addColumn(DOUBLE, &var2, "DtoF");
            addColumn(DOUBLE_HP, &var2, "Double");
            removeColumn("DtoF");
            insertColumn(1, DOUBLE, &var2, "DtoF2");
        }
    };
}