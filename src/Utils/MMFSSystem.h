#ifndef MMFS_SYSTEM_H
#define MMFS_SYSTEM_H

#include "MMFSConfig.h"

namespace mmfs
{
    class MMFSSystem
    {
    public:
        MMFSSystem(MMFSConfig *config);
        void init();
        bool update(double ms = -1); //returns true if update occurred

    private:
        bool ready = false;
        MMFSConfig *config = nullptr;
        double lastUpdate = 0;
    };
}
#endif