#include "RFM69HCW.h"

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
Most basic transmission method, simply transmits the string without modification
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

    if (!this->busy && this->mode == RM_TRANSMIT && this->modeReady)
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

void RFM69HCW::txE()
{
    this->bytesSR = 0;
    this->busy = false;
    this->setMode(RM_IDLE);
}

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
        this->rx();
    }
    else
    {
        return this->avail;
    }
}

bool RFM69HCW::setMode(RadioMode mode)
{
    this->mode = mode;
    if (this->mode == RM_TRANSMIT)
    {
        modeReady = false;
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
    message - input: latitude,longitude,altitude,speed,heading,precision,stage,t0 -> output: APRS message
- Video:
    message - input: char* filled with raw bytes -> output: char* filled with raw bytes + error checking
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
        APRSData data;

        // find each value separated in order by a comma and put in the APRSData array
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
                        strcpy(data.lat, currentVal);
                    if (currentValCount == 1 && strlen(currentVal) < 16)
                        strcpy(data.lng, currentVal);
                    if (currentValCount == 2 && strlen(currentVal) < 10)
                        strcpy(data.alt, currentVal);
                    if (currentValCount == 3 && strlen(currentVal) < 4)
                        strcpy(data.spd, currentVal);
                    if (currentValCount == 4 && strlen(currentVal) < 4)
                        strcpy(data.hdg, currentVal);
                    if (currentValCount == 5)
                        data.precision = currentVal[0];
                    if (currentValCount == 6 && strlen(currentVal) < 3)
                        strcpy(data.stage, currentVal);
                    if (currentValCount == 7 && strlen(currentVal) < 9)
                        strcpy(data.t0, currentVal);

                    currentValIndex = 0;
                    currentValCount++;
                }
            }

            delete[] currentVal;
        }
        // get lat and long string for low or high precision
        if (data.precision == 'L')
        {
            strcpy(data.dao, "");
            APRSMsg::formatLat(data.lat, 0);
            APRSMsg::formatLong(data.lng, 0);
        }
        else if (data.precision == 'H')
        {
            APRSMsg::formatDao(data.lat, data.lng, data.dao);
            APRSMsg::formatLat(data.lat, 1);
            APRSMsg::formatLong(data.lng, 1);
        }
        // get alt string
        int altInt = max(-99999, min(999999, atoi(data.alt)));
        if (altInt < 0)
        {
            strcpy(data.alt, "/A=-");
            APRSMsg::padding(altInt * -1, 5, data.alt, 4);
        }
        else
        {
            strcpy(data.alt, "/A=");
            APRSMsg::padding(altInt, 6, data.alt, 3);
        }

        // get course/speed strings
        // TODO add speed zero counter (makes decoding more complex)
        int spd_int = max(0, min(999, atoi(data.spd)));
        int hdg_int = max(0, min(360, atoi(data.hdg)));
        if (hdg_int == 0)
            hdg_int = 360;
        APRSMsg::padding(spd_int, 3, data.spd);
        APRSMsg::padding(hdg_int, 3, data.hdg);

        // generate the aprs message
        APRSMsg aprs;

        aprs.setSource(this->cfg.CALLSIGN);
        aprs.setPath(this->cfg.PATH);
        aprs.setDestination(this->cfg.TOCALL);

        char body[80];
        sprintf(body, "%c%s%c%s%c%s%c%s%s%s%s%c%s%c%s", '!', data.lat, this->cfg.OVERLAY, data.lng, this->cfg.SYMBOL,
                data.hdg, '/', data.spd, data.alt, "/S", data.stage, '/',
                data.t0, ' ', data.dao);
        aprs.getBody()->setData(body);
        aprs.encode(message);
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
- Telemetry: TODO: fix
    message - output: latitude,longitude,altitude,speed,heading,precision,stage,t0 <- input: APRS message
- Video: TODO
    message - output: char* filled with raw bytes <- input: Raw byte array
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

        APRSMsg aprs;
        aprs.decode(message);

        char body[80];
        char *bodyptr = body;
        strcpy(body, aprs.getBody()->getData());
        // decode body
        APRSData data;
        int i = 0;
        int len = strlen(body);

        // TODO this could probably be shortened
        // body should start with '!'
        if (body[0] != '!')
            return false;
        i++;
        bodyptr = body + i;

        // find latitude
        while (body[i] != '/' && i != len)
            i++;
        if (i == len)
            return false;
        strncpy(data.lat, bodyptr, i - (bodyptr - body));
        data.lat[i - (bodyptr - body)] = 0;
        i++;
        bodyptr = body + i;

        // find longitude
        while (body[i] != '[' && i != len)
            i++;
        if (i == len)
            return false;
        strncpy(data.lng, bodyptr, i - (bodyptr - body));
        data.lng[i - (bodyptr - body)] = 0;
        i++;
        bodyptr = body + i;

        // find heading
        while (body[i] != '/' && i != len)
            i++;
        if (i == len)
            return false;
        strncpy(data.hdg, bodyptr, i - (bodyptr - body));
        data.hdg[i - (bodyptr - body)] = 0;
        i++;
        bodyptr = body + i;

        // find speed
        while (body[i] != '/' && i != len)
            i++;
        if (i == len)
            return false;
        strncpy(data.spd, bodyptr, i - (bodyptr - body));
        data.spd[i - (bodyptr - body)] = 0;
        i++;
        bodyptr = body + i;

        // find altitude
        if (body[i] != 'A' && body[i + 1] != '=')
            return false;
        i += 2;
        bodyptr = body + i;
        while (body[i] != '/' && i != len)
            i++;
        if (i == len)
            return false;
        strncpy(data.alt, bodyptr, i - (bodyptr - body));
        data.alt[i - (bodyptr - body)] = 0;
        i++;
        bodyptr = body + i;

        // find stage
        if (body[i] != 'S')
            return false;
        i++;
        bodyptr = body + i;
        while (body[i] != '/' && i != len)
            i++;
        if (i == len)
            return false;
        strncpy(data.stage, bodyptr, i - (bodyptr - body));
        data.stage[i - (bodyptr - body)] = 0;
        i++;
        bodyptr = body + i;

        // find t0
        while (body[i] != ' ' && i != len)
            i++;
        if (i == len)
            return false;
        strncpy(data.t0, bodyptr, i - (bodyptr - body));
        data.t0[i - (bodyptr - body)] = 0;
        i++;
        bodyptr = body + i;

        // find dao
        strncpy(data.dao, bodyptr, len - (bodyptr - body));
        data.dao[len - (bodyptr - body)] = '\0';

        // convert lat and lng to degrees

        int latMult = (data.lat[strlen(data.lat) - 1] == 'N') ? 1 : -1;
        int lngMult = (data.lat[strlen(data.lat) - 1] == 'E') ? 1 : -1;

        int lenLat = strlen(data.lat);
        int decimalPosLat = 0;
        // use for loop in case there is no decimal
        for (int i = 0; i < lenLat; i++)
        {
            if (data.lat[i] == '.')
            {
                decimalPosLat = i;
                break;
            }
        }

        int lenLng = strlen(data.lng);
        int decimalPosLng = 0;
        // use for loop in case there is no decimal
        for (int i = 0; i < lenLng; i++)
        {
            if (data.lng[i] == '.')
            {
                decimalPosLng = i;
                break;
            }
        }

        double lat = 0;
        for (int i = decimalPosLat - 3; i >= 0; i--)
        {
            int t = data.lat[i];
            for (int j = 0; j < i; j++)
                t *= 10;
            lat += t;
        }

        double latMins = 0;
        for (int i = lenLat - 2; i > decimalPosLat - 3; i--)
        {
            if (data.lat[i] == '.')
                continue;
            double t = data.lat[i];
            for (int j = (lenLat - 2 - decimalPosLat) * -1; j < i - 2; j++)
                t *= j < 0 ? 1 / 10 : 10;
            latMins += t;
        }
        latMins /= 60;

        lat += latMins;

        double lng = 0;
        for (int i = decimalPosLng - 3; i >= 0; i--)
        {
            int t = data.lng[i];
            for (int j = 0; j < i; j++)
                t *= 10;
            lat += t;
        }

        double lngMins = 0;
        for (int i = lenLng - 2; i > decimalPosLng - 3; i--)
        {
            if (data.lng[i] == '.')
                continue;
            double t = data.lng[i];
            for (int j = (lenLng - 2 - decimalPosLng) * -1; j < i - 2; j++)
                t *= j < 0 ? 1 / 10 : 10;
            lngMins += t;
        }
        lngMins /= 60;

        lng += lngMins;

        lat *= latMult;
        lng *= lngMult;

        sprintf(message, "%f,%f,%s,%s,%s,%c,%s,%s", lat, lng, data.alt, data.spd, data.hdg, strlen(data.dao) > 0 ? 'H' : 'L', data.stage, data.t0);

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
        APRSMsg aprs;
        aprs.decode(message);
        aprs.toString(message);

        // add RSSI to the end of message
        this->RSSI();
        sprintf(message + strlen(message), "%s%d", ",RSSI:", this->rssi);

        this->msgLen = strlen(message);
        return true;
    }
    if (type == ENCT_NONE)
        return true;
    return false;
}

// maybe broken, needs to be called at a particular time
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
#endif