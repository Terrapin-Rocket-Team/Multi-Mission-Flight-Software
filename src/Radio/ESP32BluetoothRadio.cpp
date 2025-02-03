//
// Created by ramykaddouri on 2/3/25.
//

#include "ESP32BluetoothRadio.h"

mmfs::ESP32BluetoothRadio::ESP32BluetoothRadio(HardwareSerialIMXRT &port) : port(port) {

}

mmfs::ESP32BluetoothRadio::~ESP32BluetoothRadio() {
}

bool mmfs::ESP32BluetoothRadio::begin() {
    port.begin(9600);
    return false;
}

bool mmfs::ESP32BluetoothRadio::tx(const uint8_t *message, int len) {
    port.write(message, len);
    return false;
}

bool mmfs::ESP32BluetoothRadio::rx() {
    // uint8_t buf[1024];
    // port.readBytes(buf, 1024);
    return false;
}

bool mmfs::ESP32BluetoothRadio::send(Data &data) {
    Message message;
    data.encode(message.buf, message.size);
    return false;
}

bool mmfs::ESP32BluetoothRadio::receive(Data &data) {
    return false;
}

int mmfs::ESP32BluetoothRadio::RSSI() {
    return 0;
}
