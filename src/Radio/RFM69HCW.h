#ifndef RFM69HCW_H
#define RFM69HCW_H

#if defined(ARDUINO)
#define MSG_LEN 200
#elif defined(TEENSYDUINO)
#define MSG_LEN 10 * 1024
#elif defined(RASPBERRY_PI) || defined(__unix__)
#define MSG_LEN 10 * 1024
#else
#define MSG_LEN 200
#endif

#ifndef MSG_LEN
#define MSG_LEN 1000
#endif

#define SYNC1 0x00
#define SYNC2 0xff

#include "Radio.h"
#include "APRS/APRSMsg.h"
#include "RH_RF69.h"
namespace mmfs
{
    class RFM69HCW : public Radio
    {
        struct RadioSettings
        {
            double frequency;
            RH_RF69::ModemConfigChoice bitrate;
            int txPower;
            uint8_t addr;
            uint8_t toAddr;
            bool useInterrupts;
            bool highBitrate;
            RHGenericSPI *spi;
            uint8_t cs;
            uint8_t irq;
            uint8_t rst;
            uint8_t ff;
            uint8_t fne;
            uint8_t fl;
        };

    public:
        RFM69HCW(const RadioSettings *s, const APRSConfig *config);
        bool begin() override;
        bool tx(const char *message, int len = -1) override;
        void txI();
        void txE();
        const char *rx() override;
        void rxI();
        bool encode(char *message, EncodingType type, int len = -1) override;
        bool decode(char *message, EncodingType type, int len = -1) override;
        bool send(const char *message, EncodingType type, int len = -1) override;
        const char *receive(EncodingType type) override;
        int RSSI() override;
        bool available();
        void set300KBPS();
        bool setMode(RadioMode mode);
        bool checkModeReady();

        // stores full messages, max length determined by platform
        char msg[MSG_LEN + 1];
        // length of msg for recieving binary messages
        uint16_t msgLen = 0;
        int bytesToSR = 0; // bytes to Send/Receive
        int bytesSR = 0;   // bytes Sent/Received

        RH_RF69 radio;

    private:
        // interrupt methods
        static void i0();
        static void i1();
        static void i2();
        static void i3();
        static void ifl0();
        static void ifl1();
        static void ifl2();
        static void ifl3();

        // read interrupt pins
        bool FifoFull();
        bool FifoNotEmpty();
        bool FifoLevel();

        // all radios should have the same networkID
        const uint8_t networkID = 0x01;
        const uint8_t sw[4] = {0xff, 0x00, 0x2d, 0xd4};

        // set by constructor
        uint8_t id;
        RadioSettings settings;

        APRSConfig cfg;
        bool avail;
        bool busy;
        bool modeReady;
        RadioMode mode;
        int rssi;

        // interrupt vars
        static RFM69HCW *devices[];
        static int numInts;
    };
} // namespace mmfs
#endif // RFM69HCW_H