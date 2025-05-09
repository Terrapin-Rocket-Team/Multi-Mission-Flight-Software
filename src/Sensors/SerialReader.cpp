#include "SerialReader.h"
namespace mmfs
{
    SerialReader::SerialReader(const char *dataPointer)
    {
        initialized = true;
        this->dataPointer = dataPointer;
        // getLogger().recordLogData(INFO_, "SerialReader: Initialized");
    }

    bool SerialReader::readColumnHeaders(int &numCols, std::string colNames[])
    {
        if (lineIdx == 0)
        {
            std::string line(dataPointer);
            if (line.at(line.length() - 1) != ',')
                line += ",";

            numCols = 0;
            int startIdx = 0;
            for (unsigned int i = 0; i < line.length(); ++i)
            {
                if (line[i] == ',')
                {
                    colNames[numCols++] = line.substr(startIdx, i - startIdx);
                    startIdx = i + 1;
                }
            }

            lineIdx++;
            return true;
        }
        else
        {
            getLogger().recordLogData(WARNING_, "SerialReader: Headers already read!");
            return false;
        }
    }

    bool SerialReader::readLine(float *data)
    {
        std::string line(dataPointer);
        if (line.length() == 0)
            return false;
        if (line.at(line.length() - 1) != ',')
            line += ",";

        int startIdx = 0;
        size_t i = 0;
        while (i < MAX_NUM_COLS)
        {
            int commaIdx = line.find(',', startIdx);
            if (commaIdx == -1)
                break;
                std::string col = line.substr(startIdx, commaIdx - startIdx);
            data[i++] = static_cast<float>(strtod(col.c_str(), nullptr));
            startIdx = commaIdx + 1;
        }

        lineIdx++;
        return i > 0;
    }

    bool SerialReader::isInit() const
    {
        return initialized;
    }
}