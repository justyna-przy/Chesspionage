#include <Arduino.h>
#include <LiquidCrystal.h>
#include "customChessChars.h"  // Include your custom chess piece definitions

// ----------------------
// LCD Pin Definitions
// ----------------------
#define LCD_RS 2
#define LCD_E  4
#define LCD_D4 14
#define LCD_D5 15
#define LCD_D6 16
#define LCD_D7 17

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// ----------------------
// MUX Shared Pin Definitions
// ----------------------
#define MUX_S0 13
#define MUX_S1 18
#define MUX_S2 19
#define MUX_S3 21
#define MUX_SIG 34  // ADC input pin

// ----------------------
// MUX Enable Pin Definitions
// ----------------------
#define MUX1_EN 22  // MUX 1 enable pin
#define MUX2_EN 23  // MUX 2 enable pin

const int buttonPin = 12;  // Using pin 12 for the button


// ----------------------
// ADC and Sensor Conversion Constants
// ----------------------
// For ESP32 ADC (12-bit) with a 3.3V supply:
const float SUPPLY_VOLTAGE = 3.3;     // volts
const int ADC_MAX = 4095;             // 12-bit resolution
const float ZERO_OFFSET_V = 1.65;     // zero field voltage (approx half of 3.3V)
// Threshold (in volts) to decide if a magnet is present
const float DETECTION_THRESHOLD = 0.1;  // adjust as needed

// Number of samples for averaging (reduces noise)
const int NUM_SAMPLES = 50;

// ----------------------
// Function to select the desired MUX channel (0-15)
// ----------------------
void selectMuxChannel(uint8_t channel) {
  digitalWrite(MUX_S0, channel & 0x01);
  digitalWrite(MUX_S1, (channel >> 1) & 0x01);
  digitalWrite(MUX_S2, (channel >> 2) & 0x01);
  digitalWrite(MUX_S3, (channel >> 3) & 0x01);
}

// Function to read a sensor on a given MUX and channel
float readSensor(uint8_t muxEnablePin, uint8_t channel) {
  long sum = 0;
  digitalWrite(muxEnablePin, HIGH);
  selectMuxChannel(channel);
  delay(10);  // allow settling
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(MUX_SIG);
    delay(5);
  }
  digitalWrite(muxEnablePin, LOW);
  return (float)sum / NUM_SAMPLES;
}

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Starting...");

  // Load custom chess piece characters into LCD CGRAM
  lcd.createChar(0, knight);
  lcd.createChar(1, rook);
  lcd.createChar(2, bishop);
  lcd.createChar(3, queen);

  // Set MUX address pins as outputs
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  // Set MUX enable pins as outputs
  pinMode(MUX1_EN, OUTPUT);
  pinMode(MUX2_EN, OUTPUT);

  // Initially disable both MUXes
  digitalWrite(MUX1_EN, LOW);
  digitalWrite(MUX2_EN, LOW);

  pinMode(buttonPin, INPUT_PULLUP);

  delay(1000);
}

void loop() {

  if (digitalRead(buttonPin) == LOW) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Button Pressed!");
    delay(1000);  // Display message for 1 second
  }


  // --- Read Sensor 1 from MUX1, Channel 7 ---
  float adcAvg1 = readSensor(MUX1_EN, 7);
  // --- Read Sensor 2 from MUX2, Channel 11 ---
  float adcAvg2 = readSensor(MUX2_EN, 11);

  // Convert averaged ADC values to voltage
  float voltage1 = adcAvg1 * (SUPPLY_VOLTAGE / (ADC_MAX + 1.0));
  float voltage2 = adcAvg2 * (SUPPLY_VOLTAGE / (ADC_MAX + 1.0));

  // Determine if magnet is present and its polarity
  // Polarity: voltage > ZERO_OFFSET_V => positive, voltage < ZERO_OFFSET_V => negative
  bool magnet1Present = fabs(voltage1 - ZERO_OFFSET_V) > DETECTION_THRESHOLD;
  bool magnet2Present = fabs(voltage2 - ZERO_OFFSET_V) > DETECTION_THRESHOLD;

  String polarity1 = (voltage1 > ZERO_OFFSET_V) ? "Pos" : "Neg";
  String polarity2 = (voltage2 > ZERO_OFFSET_V) ? "Pos" : "Neg";

  // --- Display the sensor readings and detection info on the LCD ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("S1: ");
  lcd.print(voltage1, 2);
  if (magnet1Present) {
    lcd.print(" ");
    lcd.print(polarity1);
  } else {
    lcd.print(" NoMag");
  }

  lcd.setCursor(0, 1);
  lcd.print("S2: ");
  lcd.print(voltage2, 2);
  if (magnet2Present) {
    lcd.print(" ");
    lcd.print(polarity2);
  } else {
    lcd.print(" NoMag");
  }
  delay(1500); // Show sensor readings for 1.5 sec

  // --- Display the custom chess pieces ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pieces:");
  lcd.setCursor(0, 1);
  lcd.write(byte(0)); // Knight
  lcd.write(byte(1)); // Rook
  lcd.write(byte(2)); // Bishop
  lcd.write(byte(3)); // Queen
  delay(1300); // Show chess pieces for 3 sec
}
