#include <LiquidCrystal.h>
#include <Arduino.h>

// --- Multiplexer Pin Definitions ---
const int S0_PIN = 14;    // MUX address bit 0
const int S1_PIN = 27;    // MUX address bit 1
const int S2_PIN = 26;    // MUX address bit 2
const int S3_PIN = 25;    // MUX address bit 3

// Four enable pins (active LOW) for the four MUXes
const int EN_PIN[4] = {17, 18, 19, 21};

// --- LCD Pin Definitions ---
// From your diagram: 
// RS: ESP32 GPIO4, E: ESP32 GPIO16, D4: GPIO32, D5: GPIO33, D6: GPIO34, D7: GPIO35
LiquidCrystal lcd(4, 16, 32, 33, 34, 35);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting 4x16-channel MUX LED cycle with LCD display...");

  // Initialize MUX address pins as outputs
  pinMode(S0_PIN, OUTPUT);
  pinMode(S1_PIN, OUTPUT);
  pinMode(S2_PIN, OUTPUT);
  pinMode(S3_PIN, OUTPUT);

  // Initialize MUX enable pins as outputs; disable all initially (active LOW means HIGH disables)
  for (int i = 0; i < 4; i++) {
    pinMode(EN_PIN[i], OUTPUT);
    digitalWrite(EN_PIN[i], HIGH); // disable mux i
  }

  // Initialize the LCD (16 columns x 2 rows)
  lcd.begin(16, 2);
  lcd.clear();
}

void setMuxChannel(int channel) {
  // Set the 4-bit address for the multiplexer channel (0..15)
  digitalWrite(S0_PIN, (channel & 0x01) ? HIGH : LOW);
  digitalWrite(S1_PIN, (channel & 0x02) ? HIGH : LOW);
  digitalWrite(S2_PIN, (channel & 0x04) ? HIGH : LOW);
  digitalWrite(S3_PIN, (channel & 0x08) ? HIGH : LOW);
}

void loop() {
  // Cycle through each of the 4 multiplexers (4 MUXes x 16 channels = 64 channels total)
  for (int muxIndex = 0; muxIndex < 4; muxIndex++) {
    // Enable only the current MUX (active LOW) and disable the others
    for (int m = 0; m < 4; m++) {
      digitalWrite(EN_PIN[m], (m == muxIndex) ? LOW : HIGH);
    }

    // Now cycle through all 16 channels on the current MUX
    for (int ch = 0; ch < 16; ch++) {
      setMuxChannel(ch);

      // Update the LCD with the message
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MUX ");
      lcd.print(muxIndex);
      lcd.print(", ch ");
      lcd.print(ch);
      lcd.setCursor(0, 1);
      lcd.print("LED on");

      // Also output to Serial Monitor
      Serial.print("MUX ");
      Serial.print(muxIndex);
      Serial.print(", channel ");
      Serial.print(ch);
      Serial.println(" is active (LED on).");

      delay(500);  // Delay to allow time to view the message on the LCD
    }
  }
}
