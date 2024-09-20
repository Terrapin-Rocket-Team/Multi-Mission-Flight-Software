#ifndef APRS_CMD_MSG_H
#define APRS_CMD_MSG_H

#include "APRSMsgBase.h"
namespace mmfs
{
    struct APRSCmdData
    {
        uint8_t minutesUntilPowerOn;       // Minutes until the Pi turns on - 0 means turn on now
        uint8_t minutesUntilVideoStart;    // Minutes until the Pi starts recording video - 0 means start now
        uint8_t minutesUntilDataRecording; // Minutes until the FC starts recording data - 0 means start now
        bool launch;                       // Send the launch command to the FC - 0 means don't send
    };

    class APRSCmdMsg : public APRSMsgBase
    {
    public:
        APRSCmdMsg(APRSConfig header);
        APRSCmdData data;
        const char *type = "Text";

    protected:
        void decodeData(int cursor) override;
        void encodeData(int cursor) override;
    };
} // namespace mmfs
#endif // APRS_CMD_MSG_H