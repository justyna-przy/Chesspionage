#include <Arduino.h>

// Define multiplexer control pins
#define S0 25  // D2 on FireBeetle ESP32
#define S1 26  // D3 on FireBeetle ESP32
#define S2 27  // D4 on FireBeetle ESP32 (not needed for C0-C3 but wired)
#define S3 9   // D5 on FireBeetle ESP32 (not needed for C0-C3 but wired)
#define SIG 10 // D6 on FireBeetle ESP32 - common signal output

void setup() {
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(SIG, OUTPUT);
    
    digitalWrite(SIG, LOW); // Ensure LEDs are off at start
}

void selectChannel(int channel) {
    digitalWrite(S0, channel & 0x01);
    digitalWrite(S1, channel & 0x02);
    digitalWrite(S2, channel & 0x04);
    digitalWrite(S3, channel & 0x08);
}

void loop() {
    for (int i = 0; i < 16; i++) { // Cycle through C0-C3
        selectChannel(i); 
        digitalWrite(SIG, HIGH); // Turn on the selected LED
        delay(10);
        digitalWrite(SIG, LOW);  // Turn off the LED
        delay(10);
    }
}
