//
// Created by ramykaddouri on 2/3/25.
//

#include "ESP32BluetoothRadio.h"

#include <utility>

mmfs::ESP32BluetoothRadio::ESP32BluetoothRadio(HardwareSerialIMXRT &port, std::string name, bool hangForSerialReadyOnInit) : port(port), name(std::move(name)) {
    this->hangForSerialOnInit = hangForSerialReadyOnInit;
}

mmfs::ESP32BluetoothRadio::~ESP32BluetoothRadio() {
}

bool mmfs::ESP32BluetoothRadio::begin() {
    port.begin(9600);

    if(hangForSerialOnInit) {
        while(!port) {
            Serial.println("Waiting for serial now...");
            delay(500);
        }
        Serial.println("Serial ready");
    }

    Serial.println("Writing the following name to the ESP32:");
    Serial.println(name.c_str());


    if (port.write(INIT_MESSAGE) <= 0) return false;
    if (port.write(name.c_str()) < name.size()) return false;
    port.flush();

    return true;
}

bool mmfs::ESP32BluetoothRadio::tx(const uint8_t *message, int len) {
    port.write(DATA_MESSAGE);
    uint16_t messageSize = len;
    port.write(reinterpret_cast<uint8_t*>(&messageSize), sizeof(uint16_t));
    port.write(message, messageSize);
    port.flush();
    return true;
}

bool mmfs::ESP32BluetoothRadio::rx() {
    if (!port.available()) {
        receiveBufferSize = 0;
        return false;
    }

    Serial.println("Received data from ESP32:");

    uint8_t messageType = port.read();
    Serial.println("Message type: " + String(messageType));

    if (messageType == DATA_MESSAGE) {
        uint16_t size = -1;
        while (!port.available()) {}
        port.readBytes(reinterpret_cast<char*>(&size), sizeof(size));

        Serial.println("Data size received: " + String(size));

        if (size > 0 && size <= RECEIVE_BUFFER_SIZE) {
            receiveBufferSize = size;

            while (!port.available()) {}
            port.readBytes(receiveBuffer, size);

            Serial.println("Received message: ");
            // Serial.println("received size: " + String(receiveBufferSize));
            for (uint8_t i = 0; i < receiveBufferSize; i++) {
                Serial.print(receiveBuffer[i]);
            }
            Serial.println("");

            return true;
        } else {
            Serial.println("Invalid message message size");
        }
    } else if (messageType == STATUS_MESSAGE) {
        while (!port.available()) {
            delay(10);
        }
        uint8_t status = port.read();
        Serial.println("RECEIVED STATUS message: " + String(status));
        if (status == 1) {
            ready = true;
        } else {
            ready = false;
        }
    }

    receiveBufferSize = 0;
    return false;
}

bool mmfs::ESP32BluetoothRadio::send(Data &data) {
    Message message;
    data.encode(message.buf, message.size);
    return tx(message.buf, message.size);
}

bool mmfs::ESP32BluetoothRadio::receive(Data &data) {
    if (rx()) {
        data.encode(receiveBuffer, receiveBufferSize);
        return true;
    }
    return false;
}

int mmfs::ESP32BluetoothRadio::RSSI() {
    return 0;
}

bool mmfs::ESP32BluetoothRadio::isReady() {
    return ready;
}

