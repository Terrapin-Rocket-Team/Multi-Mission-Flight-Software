//
// Created by ramykaddouri on 2/3/25.
//

#ifndef BLUETOOTHRADIO_H
#define BLUETOOTHRADIO_H
#include "Radio.h"
#include <string>

#define INIT_MESSAGE 1
#define DATA_MESSAGE 2
#define STATUS_MESSAGE 3
#define RECEIVE_BUFFER_SIZE 512

namespace mmfs {
    class ESP32BluetoothRadio : public Radio {
    private:
        HardwareSerialIMXRT &port;
        std::string name;
        uint8_t receiveBuffer[RECEIVE_BUFFER_SIZE]{};
        uint16_t receiveBufferSize = 0;
        bool hangForSerialOnInit = true;
        bool ready = false;

    public:
        // name here can either be the server name when used with an
        // ESP32 module configured in server mode or the name of the
        // server device to connect to when used with an ESP32 module
        // in client mode.
        ESP32BluetoothRadio(HardwareSerialIMXRT &port, std::string name, bool hangForSerialOnInit = false);

        ~ESP32BluetoothRadio() override;

        bool begin() override;

        bool tx(const uint8_t *message, int len = -1) override;

        bool rx() override;

        bool send(Data &data) override;

        bool receive(Data &data) override;

        int RSSI() override;

        // For a server: will return true if everything has been successfully initialized on the ESP32 side
        // For a client: will return true if the client has successfully initialized and established a connection the server
        bool isReady();

        const uint8_t *getReceiveBuffer() const;
        const uint16_t getReceiveSize() const;
    };
}


#endif //BLUETOOTHRADIO_H
