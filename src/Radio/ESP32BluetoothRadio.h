//
// Created by ramykaddouri on 2/3/25.
//

#ifndef BLUETOOTHRADIO_H
#define BLUETOOTHRADIO_H
#include "Radio.h"

namespace mmfs {
class ESP32BluetoothRadio : public Radio {
public:
    ESP32BluetoothRadio();
    ~ESP32BluetoothRadio() override {

    }
    bool begin() override {
        return false;
    }
    bool tx(const uint8_t *message, int len = -1) override {};
    bool rx() override {};
    bool send(Data &data) override {};
    bool receive(Data &data) override {};
    int RSSI() override {};
};
}


#endif //BLUETOOTHRADIO_H
