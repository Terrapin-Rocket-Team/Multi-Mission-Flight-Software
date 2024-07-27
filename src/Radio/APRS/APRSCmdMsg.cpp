#include "APRSCmdMsg.h"

namespace mmfs
{
    APRSCmdMsg::APRSCmdMsg(APRSConfig header) : APRSMsgBase(header)
    {
        data.minutesUntilPowerOn = -1;
        data.minutesUntilVideoStart = -1;
        data.minutesUntilDataRecording = -1; // -1 so that we know if the value was set or not
        data.launch = false;
    }

    void APRSCmdMsg::encodeData(int cursor)
    {
        snprintf((char *)&string[cursor], RADIO_MESSAGE_BUFFER_SIZE - cursor,
                 ":%-9s:PWR:%d VID:%d DAT:%d L:%d",
                 header.CALLSIGN,
                 data.minutesUntilPowerOn,
                 data.minutesUntilVideoStart,
                 data.minutesUntilDataRecording,
                 data.launch);
        len = strlen((char *)string);
    }

    void APRSCmdMsg::decodeData(int cursor)
    {
        sscanf((char *)&string[cursor + 11], // ignore the :TO_FIELD: part
               "PWR:%d VID:%d DAT:%d L:%d",
               &data.minutesUntilPowerOn,
               &data.minutesUntilVideoStart,
               &data.minutesUntilDataRecording,
               &data.launch);
    }
} // namespace mmfs