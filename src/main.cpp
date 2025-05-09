//
// Created by ramykaddouri on 3/24/25.
//

#include <Arduino.h>
#include <Radio/ESP32BluetoothRadio.h>
#include <MMFS.h>


#define BTSERIAL Serial1
mmfs::ESP32BluetoothRadio btRadio(BTSERIAL, "AVIONICS");

void setup() {
    Serial.begin(9600);
    btRadio.begin();
    delay(1000);

    // while (!btRadio.isReady()) {
    //     delay(500);
    //     // btRadio.rx();
    //     Serial.print(".");
    // }
    // Serial.println("");
    btRadio.rx();

    Serial.println("Sending message...");
    const uint8_t msg[] = "Hello World!";
    btRadio.tx(msg, sizeof(msg));
    Serial.println("Sent");
}

uint32_t lastSend = 0;

void loop() {
    btRadio.rx();

    if (millis() - lastSend >= 1000) {
        const uint8_t msg[] = "Hello from the server!";
        Serial.println("Sending...");
        btRadio.tx(msg, sizeof(msg));
        Serial.println("Sent!");
        lastSend = millis();

        Serial.println("Receive buffer size: " + String(btRadio.getReceiveBufferSize()));
        Serial.print("\tRecieve buffer: ");
        for (int i = 0; i < btRadio.getReceiveBufferSize(); i++) {
            Serial.print((char) btRadio.getReceiveBuffer()[i]);
        }
        Serial.println();
    }
}