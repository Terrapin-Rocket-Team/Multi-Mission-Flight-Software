// //
// // Created by ramykaddouri on 3/24/25.
// //

// #include <Arduino.h>
// #include <Radio/ESP32BluetoothRadio.h>
// #include <MMFS.h>


// #define BTSERIAL Serial1
// mmfs::ESP32BluetoothRadio btRadio(BTSERIAL, "TRT_BT");

// void setup() {
//     Serial.begin(9600);
//     Serial.println("Hello World!");
//     btRadio.begin();

//     while (!btRadio.isReady()) {
//         delay(500);
//         btRadio.rx();
//         Serial.print(".");
//     }
//     Serial.println("");

//     Serial.println("Sending message...");
//     const uint8_t msg[] = "Hello World!";
//     btRadio.tx(msg, sizeof(msg));
// }

// void loop() {
//     btRadio.rx();
// }