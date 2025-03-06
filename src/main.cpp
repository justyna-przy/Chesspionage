#include <Arduino.h>
#include <LiquidCrystal.h>

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

// ----------------------
// Function to select the desired channel (0-15)
// ----------------------
void selectMuxChannel(uint8_t channel) {
  digitalWrite(MUX_S0, channel & 0x01);
  digitalWrite(MUX_S1, (channel >> 1) & 0x01);
  digitalWrite(MUX_S2, (channel >> 2) & 0x01);
  digitalWrite(MUX_S3, (channel >> 3) & 0x01);
}

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Starting...");
  
  // Set MUX address lines as outputs
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
  
  delay(1000);
}

void loop() {
  int sensor1 = 0;
  int sensor2 = 0;
  
  // --- Read from MUX 1, Channel 7 ---
  // Enable MUX 1 and disable MUX 2
  digitalWrite(MUX1_EN, HIGH);
  digitalWrite(MUX2_EN, LOW);
  
  // Set channel 7 (binary 0111)
  selectMuxChannel(7);
  delay(10);  // allow signal to settle
  sensor1 = analogRead(MUX_SIG);
  
  // Disable MUX 1 after reading
  digitalWrite(MUX1_EN, LOW);
  
  // --- Read from MUX 2, Channel 11 ---
  // Disable MUX 1 and enable MUX 2
  digitalWrite(MUX1_EN, LOW);
  digitalWrite(MUX2_EN, HIGH);
  
  // Set channel 11 (binary 1011)
  selectMuxChannel(11);
  delay(10);  // allow signal to settle
  sensor2 = analogRead(MUX_SIG);
  
  // Disable MUX 2 after reading
  digitalWrite(MUX2_EN, LOW);
  
  // --- Display the sensor readings on the LCD ---
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("MUX1 C7: ");
  lcd.print(sensor1);
  
  lcd.setCursor(0, 1);
  lcd.print("MUX2 C11: ");
  lcd.print(sensor2);
  
  delay(500);  // update every half second
}



/*
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
*/