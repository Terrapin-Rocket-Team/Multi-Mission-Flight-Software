//
// Created by ramykaddouri on 2/3/25.
//

#include "ESP32BluetoothRadio.h"

#include <utility>

mmfs::ESP32BluetoothRadio::ESP32BluetoothRadio(HardwareSerialIMXRT &port, std::string name) : port(port), name(std::move(name)) {}

mmfs::ESP32BluetoothRadio::~ESP32BluetoothRadio() {
}

bool mmfs::ESP32BluetoothRadio::begin() {
    port.begin(9600);

    if (port.write(INIT_MESSAGE) <= 0) return false;
    if (port.write(name.c_str()) < name.size()) return false;

    return true;
}

bool mmfs::ESP32BluetoothRadio::tx(const uint8_t *message, int len) {
    port.write(DATA_MESSAGE);
    port.write(static_cast<uint16_t>(len));
    port.write(message, len);
    return true;
}

bool mmfs::ESP32BluetoothRadio::rx() {
    if (port.available() > 0) {
        uint16_t size = -1;
        port.readBytes(reinterpret_cast<char*>(&size), sizeof(size));

        if (size > 0) {
            receiveBufferSize = size;
            port.readBytes(receiveBuffer, size);
            return true;
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
