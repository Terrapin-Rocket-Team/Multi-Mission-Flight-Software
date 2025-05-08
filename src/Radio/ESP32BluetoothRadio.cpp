//
// Created by ramykaddouri on 2/3/25.
//

#include "ESP32BluetoothRadio.h"

#include <utility>

mmfs::ESP32BluetoothRadio::ESP32BluetoothRadio(HardwareSerialIMXRT &port, std::string name, bool hangForSerialReadyOnInit) : port(port), name(std::move(name))
{
    this->hangForSerialOnInit = hangForSerialReadyOnInit;
}

mmfs::ESP32BluetoothRadio::~ESP32BluetoothRadio()
{
}

bool mmfs::ESP32BluetoothRadio::begin()
{
    port.begin(9600);

    if (hangForSerialOnInit)
    {
        while (!port)
        {
            Serial.println("Waiting for serial now...");
            delay(500);
        }
        Serial.println("Serial ready");
    }

    Serial.println("Writing the following name to the ESP32:");
    Serial.println(name.c_str());

    if (port.write(INIT_MESSAGE) <= 0)
        return false;
    if (port.write(name.c_str()) + port.write('\0') < name.size())
        return false;
    port.flush();

    return true;
}

bool mmfs::ESP32BluetoothRadio::tx(const uint8_t *message, int len)
{
    port.write(DATA_MESSAGE);
    uint16_t messageSize = len;
    port.write(reinterpret_cast<uint8_t *>(&messageSize), sizeof(uint16_t)); // prints both bits of the short to serial
    port.write(message, messageSize);                                        // this is kinda crazy lol.
    port.flush();
    return true;
}

void waitForSerial(uint32_t ms, Stream &s)
{
    uint32_t start = millis();
    while (millis() - start < ms)
    {
        if (s.available())
            break;
        delay(1);
    }
}

bool mmfs::ESP32BluetoothRadio::rx()
{
    uint8_t messageType = port.read();
    if (messageType == 255)
        return false;

    // Serial.println("Received data from ESP32:");

    // Serial.println("Message type: " + String(messageType));

    if (messageType == DATA_MESSAGE)
    {
        uint16_t size = -1;
        waitForSerial(100, port);
        port.readBytes(reinterpret_cast<char *>(&size), sizeof(size));

        Serial.println("Data size received: " + String(size));

        if (size > 0 && size <= RECEIVE_BUFFER_SIZE)
        {
            receiveBufferSize = size;

            waitForSerial(100, port);
            port.readBytes(receiveBuffer, size);

            Serial.println("Received message: ");
            Serial.println("received size: " + String(receiveBufferSize));
            for (uint8_t i = 0; i < receiveBufferSize; i++)
            {
                Serial.print((char)receiveBuffer[i]);
            }
            Serial.println("");

            return true;
        }
        else
        {
            Serial.println("Invalid message message size");
        }
    }
    else if (messageType == STATUS_MESSAGE)
    {
        waitForSerial(100, port);
        uint8_t status = port.read();
        Serial.println("RECEIVED STATUS message: " + String(status));
        if (status == 1)
        {
            ready = true;
        }
        else
        {
            ready = false;
        }
    }
    else
    {
        // Serial.println(port.readStringUntil('\n'));
        port.readStringUntil('\n');
    }
    receiveBufferSize = 0;
    return false;
}

bool mmfs::ESP32BluetoothRadio::send(Data &data)
{
    Message message;
    data.encode(message.buf, message.size);
    return tx(message.buf, message.size);
}

bool mmfs::ESP32BluetoothRadio::receive(Data &data)
{
    if (receiveBufferSize > 0)
    {
        data.encode(receiveBuffer, receiveBufferSize);
        receiveBufferSize = 0;
        return true;
    }
    return false;
}

int mmfs::ESP32BluetoothRadio::RSSI()
{
    return 0;
}

bool mmfs::ESP32BluetoothRadio::isReady()
{
    return ready;
}

const uint8_t *mmfs::ESP32BluetoothRadio::getReceiveBuffer() const
{
    return receiveBuffer;
}

const uint16_t mmfs::ESP32BluetoothRadio::getReceiveSize() const
{
    return receiveBufferSize;
}
