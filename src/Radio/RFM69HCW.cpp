#include "RFM69HCW.h"
namespace mmfs
{
#ifndef max
    int max(int a, int b);
#endif

#ifndef min
    int min(int a, int b);
#endif

    // Have to intialize these here, but these vars are static
    RFM69HCW *RFM69HCW::devices[] = {0, 0, 0, 0};
    int RFM69HCW::numInts = 0;

    /*
    Constructor
        - frequency in hertz
        - transmitter true if the radio will be on the rocket
        - highBitrate true for 300kbps, false for 4.8kbps
        - config with APRS settings
    */
    RFM69HCW::RFM69HCW(const RadioSettings *s, const APRSConfig *config) : radio(s->cs, s->irq, *s->spi)
    {

        this->settings = *s;
        this->cfg = *config;
        this->avail = false;
        this->msgLen = -1;
    }

    /*
    Initializer to call in setup
    */
    bool RFM69HCW::begin()
    {
        // reset the radio
        pinMode(this->settings.rst, OUTPUT);
        digitalWrite(this->settings.rst, LOW);
        delay(10);
        digitalWrite(this->settings.rst, HIGH);
        delay(10);
        digitalWrite(this->settings.rst, LOW);

        pinMode(this->settings.irq, INPUT);
        pinMode(this->settings.fne, INPUT);
        pinMode(this->settings.ff, INPUT);
        pinMode(this->settings.fl, INPUT);

        if (!this->radio.init())
            return false;

        // then use this to actually set the frequency
        if (!this->radio.setFrequency(this->settings.frequency))
            return false;

        // set transmit power
        this->radio.setTxPower(this->settings.txPower, true);

        // the default bitrate of 4.8kbps should be fine unless we want high bitrate for video
        this->radio.setModemConfig(this->settings.bitrate);
        if (this->settings.highBitrate)
        {
            this->set300KBPS();
            this->radio.setPreambleLength(100);
            this->radio.setSyncWords(this->sw, 4);
        }

        // configure unlimited packet length mode
        this->radio.spiWrite(0x37, 0b00000000);                // Packet format (0x37) set to 00000000 (see datasheet for meaning of each bit)
        this->radio.spiWrite(RH_RF69_REG_38_PAYLOADLENGTH, 0); // Payload length (0x38) set to 0
        this->radio.spiWrite(0x3C, 0b10100001);                // set FifoThresh to trigger when there are 33 bytes in the fifo

        // remove the default interrupt
        detachInterrupt(digitalPinToInterrupt(this->settings.irq));

        if (this->settings.useInterrupts)
        {

            // Check to make sure we can add another device if (numInts > 3) return false;

            // I don't like this, but it seems this is how it has to be done attach interrupts on FifoLevel
            if (numInts == 0)
            {
                attachInterrupt(digitalPinToInterrupt(this->settings.fl), ifl0, RISING);
                attachInterrupt(digitalPinToInterrupt(this->settings.irq), i0, RISING);
            }
            else if (numInts == 1)
            {
                attachInterrupt(digitalPinToInterrupt(this->settings.fl), ifl1, RISING);
                attachInterrupt(digitalPinToInterrupt(this->settings.irq), i1, RISING);
            }
            else if (numInts == 2)
            {
                attachInterrupt(digitalPinToInterrupt(this->settings.fl), ifl2, RISING);
                attachInterrupt(digitalPinToInterrupt(this->settings.irq), i2, RISING);
            }
            else if (numInts == 3)
            {
                attachInterrupt(digitalPinToInterrupt(this->settings.fl), ifl3, RISING);
                attachInterrupt(digitalPinToInterrupt(this->settings.irq), i3, RISING);
            }

            devices[numInts] = this;
            numInts++;
        }

        return true;
    }

    /*
    Most basic transmission method, starts transmission of the string without modification
        \param message is the message to be transmitted, set to nullptr if this->msg already contains the message
        \param len [optional] is the length of the message, if not used it is assumed this->msgLen is already set to the length
    */
    bool RFM69HCW::tx(const char *message, int len)
    {
        // figure out how long the message should be
        if (len > 0)
            this->msgLen = len > MSG_LEN ? MSG_LEN : len;
        else if (this->msgLen == -1)
            return false;

        if (message != nullptr)
            memcpy(this->msg, message, len);

        if (!this->busy)
        {
            this->busy = true;
            this->bytesToSR = this->msgLen;

            // Start spi transaction
            settings.spi->beginTransaction();
            // Select radio
            digitalWrite(settings.cs, LOW);

            // Select the fifo for writing
            settings.spi->transfer(RH_RF69_REG_00_FIFO | RH_RF69_SPI_WRITE_MASK);
            // Send manual sync bytes
            settings.spi->transfer(SYNC1);
            settings.spi->transfer(SYNC2);
            // Send message len
            settings.spi->transfer(msgLen >> 8);   // first 8 bits of uint16_t (from left)
            settings.spi->transfer(msgLen & 0xff); // last 8 bits of uint16_t

            // Send the payload
            this->bytesSR = 0;
            int sent = 0;
            while (this->bytesToSR > 0 && !this->FifoFull())
            {
                settings.spi->transfer(this->msg[sent]);
                sent++;
                this->bytesToSR--;
            }
            this->bytesSR = sent;

            // Deselect radio
            digitalWrite(settings.cs, HIGH);
            // End spi transaction
            settings.spi->endTransaction();

            this->setMode(RM_TRANSMIT);

            return true;
        }

        return false;
    }

    /*
    Transmit interrupt (txI), called by the iflX series of methods when in transmit mode to refill the radio fifo
    */
    void RFM69HCW::txI()
    {
        if (this->busy && this->mode == RM_TRANSMIT && this->bytesToSR > 0 && !this->FifoFull())
        {
            // Start spi transaction
            settings.spi->beginTransaction();
            // Select radio
            digitalWrite(settings.cs, LOW);

            settings.spi->transfer(RH_RF69_REG_00_FIFO | RH_RF69_SPI_WRITE_MASK);

            // Send the next section of the payload
            int sent = 0;
            while (this->bytesToSR > 0 && !this->FifoFull())
            {
                settings.spi->transfer(msg[sent]);
                sent++;
                this->bytesToSR--;
            }
            this->bytesSR += sent;

            // Deselect radio
            digitalWrite(settings.cs, HIGH);
            // End spi transaction
            settings.spi->endTransaction();
        }
    }

    /*
    Transmit end (txE), should be called by the iX handler after the full message has been received
    */
    void RFM69HCW::txE()
    {
        this->bytesSR = 0;
        this->busy = false;
        this->setMode(RM_IDLE);
    }

    /*
    Basic receive method, usually called by available(), but can be called manually. Puts the radio in receive mode and gets message data if available
    */
    const char *RFM69HCW::rx()
    {
        if (this->mode != RM_RECEIVE)
        {
            this->setMode(RM_RECEIVE);
        }
        else if (this->FifoLevel() && !this->busy && this->modeReady) // wait for the fifo to contain 33 bytes
        {
            // Start spi transaction
            settings.spi->beginTransaction();
            // Select the radio
            digitalWrite(settings.cs, LOW);

            // Send the fifo address with the write mask off
            settings.spi->transfer(RH_RF69_REG_00_FIFO);

            // if found sync bytes
            if (settings.spi->transfer(0) == SYNC1 && settings.spi->transfer(0) == SYNC2)
            {
                // set up for receiving the message
                this->busy = true;
                this->msgLen = settings.spi->transfer(0) << 8 | settings.spi->transfer(0); // reconstruct msgLen
                // TODO add check for msgLen < MSG_LEN
                this->bytesSR = 0;
                this->bytesToSR = this->msgLen;

                while (this->bytesSR < this->msgLen && this->FifoNotEmpty())
                {
                    this->msg[this->bytesSR] = settings.spi->transfer(0);
                    this->bytesSR++;
                }
            }
            // reset msgLen and toAddr, end transaction
            digitalWrite(settings.cs, HIGH);
            settings.spi->endTransaction();

            // clear fifo through entering idle mode
            if (this->bytesSR == this->msgLen)
            {
                this->bytesSR = 0;
                this->busy = false;
                this->avail = true;
                this->setMode(RM_IDLE);
            }
        }
    }

    /*
    Receive interrupt (rxI), called by the iflX series of methods when in receive mode to empty the radio fifo
    */
    void RFM69HCW::rxI()
    {
        if (this->FifoNotEmpty() && this->busy && this->mode == RM_RECEIVE)
        {
            // Start spi transaction
            settings.spi->beginTransaction();
            // Select the radio
            digitalWrite(settings.cs, LOW);

            // Send the fifo address with the write mask off
            settings.spi->transfer(RH_RF69_REG_00_FIFO);

            // data
            while (this->bytesSR < this->msgLen && this->FifoNotEmpty())
            {
                this->msg[this->bytesSR] = settings.spi->transfer(0);
                this->bytesSR++;
            }

            // reset msgLen and toAddr, end transaction, and clear fifo through entering idle mode
            digitalWrite(settings.cs, HIGH);
            settings.spi->endTransaction();

            // clear fifo through entering idle mode
            if (this->bytesSR == this->msgLen)
            {
                this->bytesSR = 0;
                this->busy = false;
                this->avail = true;
                this->setMode(RM_IDLE);
            }
        }
    }

    /*
    Encodes the message into the selected type, then sends it. Transmitted and encoded message length must not exceed ```MSG_LEN```
        \param message is the message to be sent
        \param type is the encoding type
        \param len optional length of message, required if message is not a null terminated string
    \return ```true``` if the message was sent successfully
    */
    bool RFM69HCW::send(const char *message, EncodingType type, int len)
    {
        // call txs instead of tx for unlimited mode
        if (type == ENCT_TELEMETRY || type == ENCT_GROUNDSTATION)
        {
            // assume message is a valid string, but maybe it's not null terminated
            strncpy(this->msg, message, MSG_LEN);
            this->msg[MSG_LEN] = 0; // so null terminate it just in case
            // figure out what the length of the message should be
            if (len > 0 && len <= MSG_LEN)
                this->msgLen = len;
            else
                this->msgLen = strlen(this->msg);
            return encode(this->msg, type, this->msgLen) && tx(nullptr, strlen(this->msg)); // length needs to be updated since encode() modifies the message
        }
        if (type == ENCT_NONE && len != -1)
        {
            return tx(message, len);
        }
        return false;
    }

    /*
    Comprehensive receive function
    Should be called after verifying there is an available message by calling available()
    Decodes the last received message according to the type
    Received and decoded message length must not exceed MSG_LEN
        \param type is the decoding type
    */
    const char *RFM69HCW::receive(EncodingType type)
    {
        if (this->avail)
        {
            this->avail = false;
            if (!decode(this->msg, type, this->msgLen))
                return 0;

            return this->msg;
        }
    }

    /*
    Returns true if a there is a new message
    */
    bool RFM69HCW::available()
    {
        if (!this->busy)
        {
            this->checkModeReady();
            this->rx();
        }
        else
        {
            return this->avail;
        }
    }

    /*
    Sets the radio mode, options are transmit, receive, and idle. this->modeReady will be true once the radio has successfully switched modes
    */
    bool RFM69HCW::setMode(RadioMode mode)
    {
        this->mode = mode;
        if (this->mode == RM_TRANSMIT)
        {
            this->modeReady = false;
            this->radio.spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_BOOST);
            this->radio.spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_BOOST);
            this->radio.spiWrite(RH_RF69_REG_25_DIOMAPPING1, RH_RF69_DIOMAPPING1_DIO0MAPPING_00); // Set interrupt line 0 PacketSent
            uint8_t opmode = this->radio.spiRead(RH_RF69_REG_01_OPMODE);
            opmode &= ~RH_RF69_OPMODE_MODE;
            opmode |= (RH_RF69_OPMODE_MODE_TX & RH_RF69_OPMODE_MODE);
            this->radio.spiWrite(RH_RF69_REG_01_OPMODE, opmode);
            return true;
        }
        if (this->mode == RM_RECEIVE)
        {
            this->modeReady = false;
            this->radio.spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
            this->radio.spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);
            this->radio.spiWrite(RH_RF69_REG_25_DIOMAPPING1, RH_RF69_DIOMAPPING1_DIO0MAPPING_01); // Set interrupt line 0 PayloadReady
            uint8_t opmode = this->radio.spiRead(RH_RF69_REG_01_OPMODE);
            opmode &= ~RH_RF69_OPMODE_MODE;
            opmode |= (RH_RF69_OPMODE_MODE_RX & RH_RF69_OPMODE_MODE);
            this->radio.spiWrite(RH_RF69_REG_01_OPMODE, opmode);
            return true;
        }
        if (this->mode == RM_IDLE)
        {
            this->modeReady = false;
            this->radio.spiWrite(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
            this->radio.spiWrite(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);
            uint8_t opmode = this->radio.spiRead(RH_RF69_REG_01_OPMODE);
            opmode &= ~RH_RF69_OPMODE_MODE;
            opmode |= (RH_RF69_OPMODE_MODE_STDBY & RH_RF69_OPMODE_MODE);
            this->radio.spiWrite(RH_RF69_REG_01_OPMODE, opmode);
            return true;
        }
        return false;
    }

    /*
    Reads the modeReady register on the radio and sets this->modeReady to true if the radio has switched modes
    */
    bool RFM69HCW::checkModeReady()
    {
        if (this->radio.spiRead(RH_RF69_REG_27_IRQFLAGS1) & RH_RF69_IRQFLAGS1_MODEREADY)
        {
            this->modeReady = true;
        }
        return this->modeReady;
    }

    /*
    Multi-purpose encoder function
    Encodes the message into a format selected by type
    char *message is the message to be encoded
    sets this->msgLen to the encoded length of message
    - Telemetry:
        message - input: latitude,longitude,altitude,speed,heading,rot_x,rot_y,rot_z,stage,pi_on,pi_recording,data_recording -> output: APRS Telem message
    - Command:
        message - input: minutesUntilPowerOn,minutesUntilVideoStart,minutesUntilDataRecording,launch -> APRS Command message
    - Ground Station: TODO
        message - input: Source:Value,Destination:Value,Path:Value,Type:Value,Body:Value -> output: APRS message
    */
    bool RFM69HCW::encode(char *message, EncodingType type, int len)
    {
        if (type == ENCT_TELEMETRY)
        {
            if (len > 0)
                this->msgLen = len > MSG_LEN ? MSG_LEN : len;
            else if (this->msgLen == -1)
                return false;

            // holds the data to be assembled into the aprs body
            APRSTelemMsg m(this->cfg);
            APRSTelemData data;

            // find each value separated in order by a comma and put in the APRSTelemData array
            {
                char *currentVal = new char[this->msgLen];
                int currentValIndex = 0;
                int currentValCount = 0;
                for (int i = 0; i < msgLen; i++)
                {
                    if (message[i] != ',')
                    {
                        currentVal[currentValIndex] = message[i];
                        currentValIndex++;
                    }
                    if (message[i] == ',' || (currentValCount == 7 && i == msgLen - 1))
                    {
                        currentVal[currentValIndex] = '\0';

                        if (currentValCount == 0 && strlen(currentVal) < 16)
                            sscanf(currentVal, "%lf", &data.lat);
                        if (currentValCount == 1 && strlen(currentVal) < 16)
                            sscanf(currentVal, "%lf", &data.lng);
                        if (currentValCount == 2 && strlen(currentVal) < 10)
                            data.alt = atoi(currentVal);
                        if (currentValCount == 3 && strlen(currentVal) < 4)
                            data.spd = atoi(currentVal);
                        if (currentValCount == 4 && strlen(currentVal) < 4)
                            data.hdg = atoi(currentVal);
                        if (currentValCount == 5 && strlen(currentVal) < 4)
                            data.orientation.x() = atoi(currentVal);
                        if (currentValCount == 6 && strlen(currentVal) < 4)
                            data.orientation.y() = atoi(currentVal);
                        if (currentValCount == 7 && strlen(currentVal) < 4)
                            data.orientation.z() = atoi(currentVal);
                        if (currentValCount == 8 && strlen(currentVal) < 3)
                            data.stage = atoi(currentVal);
                        if (currentValCount == 9 && strlen(currentVal) < 3)
                            data.statusFlags |= (bool)atoi(currentVal) << 2;
                        if (currentValCount == 10 && strlen(currentVal) < 3)
                            data.statusFlags |= (bool)atoi(currentVal) << 1;
                        if (currentValCount == 11 && strlen(currentVal) < 3)
                            data.statusFlags |= (bool)atoi(currentVal);

                        currentValIndex = 0;
                        currentValCount++;
                    }
                }

                delete[] currentVal;
            }

            m.data = data;
            m.encode();
            strcpy(message, (const char *)m.getArr());

            return true;
        }
        if (type == ENCT_COMMAND)
        {
            if (len > 0)
                this->msgLen = len > MSG_LEN ? MSG_LEN : len;
            else if (this->msgLen == -1)
                return false;

            // holds the data to be assembled into the aprs body
            APRSCmdMsg m(this->cfg);
            APRSCmdData data;

            // find each value separated in order by a comma and put in the APRSCmdData array
            {
                char *currentVal = new char[this->msgLen];
                int currentValIndex = 0;
                int currentValCount = 0;
                for (int i = 0; i < msgLen; i++)
                {
                    if (message[i] != ',')
                    {
                        currentVal[currentValIndex] = message[i];
                        currentValIndex++;
                    }
                    if (message[i] == ',' || (currentValCount == 7 && i == msgLen - 1))
                    {
                        currentVal[currentValIndex] = '\0';

                        if (currentValCount == 0 && strlen(currentVal) < 4)
                            data.minutesUntilPowerOn = atoi(currentVal);
                        if (currentValCount == 1 && strlen(currentVal) < 4)
                            data.minutesUntilVideoStart = atoi(currentVal);
                        if (currentValCount == 2 && strlen(currentVal) < 4)
                            data.minutesUntilDataRecording = atoi(currentVal);
                        if (currentValCount == 3 && strlen(currentVal) < 2)
                            data.launch = (bool)atoi(currentVal);

                        currentValIndex = 0;
                        currentValCount++;
                    }
                }

                delete[] currentVal;
            }

            m.data = data;
            m.encode();
            strcpy(message, (const char *)m.getArr());

            return true;
        }
        if (type == ENCT_NONE)
            return true;
        return false;
    }

    /*
    Multi-purpose decoder function
    Decodes the message into a format selected by type
    char *message is the message to be decoded
    sets this->msgLen to the length of the decoded message
    - Telemetry:
        message - output: latitude,longitude,altitude,speed,heading,rot_x,rot_y,rot_z,stage,pi_on,pi_recording,data_recording <- input: APRS message
    - Command:
        message - minutesUntilPowerOn,minutesUntilVideoStart,minutesUntilDataRecording,launch <- APRS Command message
    - Ground Station:
        message - output: Source:Value,Destination:Value,Path:Value,Type:Value,Body:Value <- input: APRS message
    */
    bool RFM69HCW::decode(char *message, EncodingType type, int len)
    {
        if (type == ENCT_TELEMETRY)
        {
            if (len > 0)
                this->msgLen = len > MSG_LEN ? MSG_LEN : len;
            else if (this->msgLen == -1)
                return false;

            // make sure message is null terminated
            message[this->msgLen] = 0;

            APRSTelemMsg m(this->cfg);
            m.setArr((uint8_t *)message, this->msgLen);
            m.decode();

            snprintf(message, this->msgLen, "%lf,%lf,%d,%d,%d,%lf,%lf,%lf,%d,%d,%d,%d", m.data.lat, m.data.lng, m.data.alt, m.data.spd, m.data.hdg, m.data.orientation.x(), m.data.orientation.y(), m.data.orientation.z(), m.data.stage, m.data.statusFlags & 0b100, m.data.statusFlags & 0b010, m.data.statusFlags & 0b001);

            this->msgLen = strlen(message);
            return true;
        }
        if (type == ENCT_GROUNDSTATION)
        {
            if (len > 0)
                this->msgLen = len > MSG_LEN ? MSG_LEN : len;
            else if (this->msgLen == -1)
                return false;

            // make sure message is null terminated
            message[this->msgLen] = 0;

            // put the message into a APRSMessage object to decode it
            APRSTelemMsg m(this->cfg);
            m.setArr((uint8_t *)message, this->msgLen);
            m.decode();

            // add RSSI to the end of message
            this->RSSI();
            snprintf(message, this->msgLen, "Source:%s,Destination:%s,Path:%s,Type:%s,Data:%lf/%lf/%d/%d/%d/%lf/%lf/%lf/%d/%d/%d/%d,RSSI:%d", m.header.CALLSIGN, m.header.TOCALL, m.header.PATH, m.type, m.data.lat, m.data.lng, m.data.alt, m.data.spd, m.data.hdg, m.data.orientation.x(), m.data.orientation.y(), m.data.orientation.z(), m.data.stage, m.data.statusFlags & 0b100, m.data.statusFlags & 0b010, m.data.statusFlags & 0b001, this->rssi);

            this->msgLen = strlen(message);
            return true;
        }
        if (type == ENCT_COMMAND)
        {
            if (len > 0)
                this->msgLen = len > MSG_LEN ? MSG_LEN : len;
            else if (this->msgLen == -1)
                return false;

            // make sure message is null terminated
            message[this->msgLen] = 0;

            APRSCmdMsg m(this->cfg);
            APRSCmdData data;
            m.setArr((uint8_t *)message, this->msgLen);
            m.decode();

            snprintf(message, this->msgLen, "%d,%d,%d,%d", m.data.minutesUntilPowerOn, m.data.minutesUntilVideoStart, m.data.minutesUntilDataRecording, m.data.launch);
            return true;
        }
        if (type == ENCT_NONE)
            return true;
        return false;
    }

    // needs to be called at a particular time to work properly
    int RFM69HCW::RSSI()
    {
        return this->rssi = -((int8_t)(this->radio.spiRead(RH_RF69_REG_24_RSSIVALUE) >> 1));
    }

    void RFM69HCW::set300KBPS()
    {
        this->radio.spiWrite(0x03, 0x00);       // REG_BITRATEMSB: 300kbps (0x006B, see DS p20)
        this->radio.spiWrite(0x04, 0x6B);       // REG_BITRATELSB: 300kbps (0x006B, see DS p20)
        this->radio.spiWrite(0x19, 0x40);       // REG_RXBW: 500kHz
        this->radio.spiWrite(0x1A, 0x80);       // REG_AFCBW: 500kHz
        this->radio.spiWrite(0x05, 0x13);       // REG_FDEVMSB: 300khz (0x1333)
        this->radio.spiWrite(0x06, 0x33);       // REG_FDEVLSB: 300khz (0x1333)
        this->radio.spiWrite(0x29, 100);        // RSSI_THRESH: -110dBm
        this->radio.spiWrite(0x37, 0b10010000); // DC=WHITENING, CRCAUTOOFF=0
                                                //                ^^->DC: 00=none, 01=manchester, 10=whitening
    }

    // interrupt functions for FifoLevel, behavior changes based on radio mode
    void RFM69HCW::ifl0()
    {
        if (devices[0] && devices[0]->mode == RM_TRANSMIT)
            devices[0]->txI();
        if (devices[0] && devices[0]->mode == RM_RECEIVE)
            devices[0]->rxI();
    }

    void RFM69HCW::ifl1()
    {
        if (devices[1] && devices[1]->mode == RM_TRANSMIT)
            devices[1]->txI();
        if (devices[1] && devices[1]->mode == RM_RECEIVE)
            devices[1]->rxI();
    }

    void RFM69HCW::ifl2()
    {
        if (devices[2] && devices[2]->mode == RM_TRANSMIT)
            devices[2]->txI();
        if (devices[2] && devices[2]->mode == RM_RECEIVE)
            devices[2]->rxI();
    }

    void RFM69HCW::ifl3()
    {
        if (devices[3] && devices[3]->mode == RM_TRANSMIT)
            devices[3]->txI();
        if (devices[3] && devices[3]->mode == RM_RECEIVE)
            devices[3]->rxI();
    }

    // Interrupt functions for g0 pin
    void RFM69HCW::i0()
    {
        if (devices[0] && devices[0]->mode == RM_TRANSMIT)
            devices[0]->txE();
    }

    void RFM69HCW::i1()
    {
        if (devices[1] && devices[1]->mode == RM_TRANSMIT)
            devices[1]->txE();
    }

    void RFM69HCW::i2()
    {
        if (devices[2] && devices[2]->mode == RM_TRANSMIT)
            devices[2]->txE();
    }

    void RFM69HCW::i3()
    {
        if (devices[3] && devices[3]->mode == RM_TRANSMIT)
            devices[3]->txE();
    }

    // returns the state of the FifoFull interrupt
    bool RFM69HCW::FifoFull()
    {
        return digitalRead(this->settings.ff);
    }

    // returns the state of the FifoNotEmpty interrupt
    bool RFM69HCW::FifoNotEmpty()
    {
        return digitalRead(this->settings.fne);
    }

    // returns the state of the FifoLevel interrupt
    bool RFM69HCW::FifoLevel()
    {
        return digitalRead(this->settings.fl);
    }

// utility functions
#ifndef max
    int max(int a, int b)
    {
        if (a > b)
            return a;
        return b;
    }
#endif

#ifndef min
    int min(int a, int b)
    {
        if (a < b)
            return a;
        return b;
    }
} // namespace mmfs
#endif