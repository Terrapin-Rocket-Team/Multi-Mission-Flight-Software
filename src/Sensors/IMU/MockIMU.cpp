#include "MockIMU.h"

using namespace mmfs;
MockIMU::MockIMU(const char *dataPath, const std::string accColNames[3], const std::string gyroColNames[3], const std::string magColNames[3]) : IMU("MockIMU"), dataReader(dataPath)
{

    for (int i = 0; i < 3; i++)
    {
        this->accColNames[i] = accColNames[i];
        this->gyroColNames[i] = gyroColNames[i];
        this->magColNames[i] = magColNames[i];

        this->accIndices[i] = -1;
        this->gyroIndices[i] = -1;
        this->magIndices[i] = -1;
    }
    }

    bool MockIMU::init()
    {
        if (!dataReader.isInit())
            return false;

        int numCols = -1;
        std::string colNames[MAX_NUM_COLS];
        dataReader.readColumnHeaders(numCols, colNames);

        if (numCols == -1 || numCols > MAX_NUM_COLS)
        {
            getLogger().recordLogData(ERROR_, 100, "[MockIMU]: Invalid number of columns read: %d", numCols);
            return false;
        }

        for (int i = 0; i < numCols; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (colNames[i] == accColNames[j])
                {
                    accIndices[j] = i;
                }
                else if (colNames[i] == gyroColNames[j])
                {
                    gyroIndices[j] = i;
                }
                else if (colNames[i] == magColNames[j])
                {
                    magIndices[j] = i;
                }
            }
        }

        // underscores indicate skipped fields
        for (int j = 0; j < 3; j++)
        {
            if (accColNames[j] == "_")
            {
                accIndices[j] = MAX_NUM_COLS - 1;
            }
            if (gyroColNames[j] == "_")
            {
                gyroIndices[j] = MAX_NUM_COLS - 1;
            }
            if (magColNames[j] == "_")
            {
                magIndices[j] = MAX_NUM_COLS - 1;
            }
        }

        for (int i = 0; i < 3; i++)
        {
            if (accIndices[i] == -1)
            {
                getLogger().recordLogData(ERROR_, 100, "[MockIMU]: Failed to find acceleration column index for name: %s", accColNames[i].c_str());
                return false;
            }

            if (gyroIndices[i] == -1)
            {
                getLogger().recordLogData(ERROR_, 100, "[MockIMU]: Failed to find gyroscope column index for name: %s", gyroColNames[i].c_str());
                return false;
            }

            if (magIndices[i] == -1)
            {
                getLogger().recordLogData(ERROR_, 100, "[MockIMU]: Failed to find magnetometer column index for name: %s", magColNames[i].c_str());
                return false;
            }
        }

        initialized = true;
        quaternionBasedComplimentaryFilterSetup();
        setAccelBestFilteringAtStatic(.5);
        setMagBestFilteringAtStatic(.5);
        return initialized;
    }

    bool MockIMU::read()    {
        if (!dataReader.readLine(sdData))
        {
            getLogger().recordLogData(ERROR_, 100, "[MockIMU]: Failed to read data from file!");
            initialized = false;
            return false;
        }
        for (int i = 0; i < 3; i++)
        {
            measuredAcc[i] = sdData[accIndices[i]];
            measuredGyro[i] = sdData[gyroIndices[i]];
            measuredMag[i] = sdData[magIndices[i]];
        }

        quaternionBasedComplimentaryFilter(UPDATE_INTERVAL / 1000.0);
        return true;
    }
