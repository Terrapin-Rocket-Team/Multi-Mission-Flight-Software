#include "MockBarometer.h"

namespace mmfs
{
    MockBarometer::MockBarometer(const char *dataPath, const std::string &pressureColName, const std::string &temperatureColName)
        : Barometer("MockBarometer"),
          dataReader(dataPath),
          pressureColName(pressureColName),
          temperatureColName(temperatureColName)

    {
    }

    bool MockBarometer::init()
    {
        if (!dataReader.isInit())
            return false;

        int numCols = -1;
        std::string colNames[MAX_NUM_COLS];
        dataReader.readColumnHeaders(numCols, colNames);

        if (numCols == -1 || numCols > MAX_NUM_COLS)
        {
            getLogger().recordLogData(ERROR_, 100, "[MockBarometer]: Invalid number of columns read: %d", numCols);
            return false;
        }

        for (int i = 0; i < numCols; i++)
        {
            if (colNames[i] == pressureColName)
            {
                pressureColIndex = i;
            }
            else if (colNames[i] == temperatureColName)
            {
                temperatureColIndex = i;
            }
        }

        // underscores indicate skipped fields
        if (pressureColName == "_")
        {
            pressureColIndex = MAX_NUM_COLS - 1;
        }
        if (temperatureColName == "_")
        {
            temperatureColIndex = MAX_NUM_COLS - 1;
        }

        if (pressureColIndex == -1)
        {
            getLogger().recordLogData(ERROR_, 100, "[MockBarometer]: Failed to find pressure column index for name: %s", pressureColName.c_str());
            return false;
        }
        if (temperatureColIndex == -1)
        {
            getLogger().recordLogData(ERROR_, 100, "[MockBarometer]: Failed to find temperature column index for name: %s", temperatureColName.c_str());
            return false;
        }

        initialized = true;
        return true;
    }

    void MockBarometer::read()
    {
        if (!dataReader.readLine(sdData))
        {
            getLogger().recordLogData(ERROR_, 100, "[MockBarometer]: Failed to read data from file!");
            initialized = false;
            return;
        }
        pressure = sdData[pressureColIndex];
        temp = sdData[temperatureColIndex];
    }
}