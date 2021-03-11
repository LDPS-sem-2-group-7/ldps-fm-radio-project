#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>

// creating the radio and LCD objects, g_ corresponds to global
AR1010 g_radio = AR1010();
LiquidCrystal_I2C g_lcd(0x27, 16, 2);
int g_volume = 0;
int g_reClkState = 0;
int g_reDatState = 0;
int g_reLastState = 0;

// global constants
const float c_minFreq = 96.4; // c_ corresponds to constant
const float c_maxFreq = 107.9;
const int c_rotaryEncoderDat 3;
const int c_rotaryEncoderClk 2;
const int c_memButton1Pin 8;
const int c_memButton2Pin 9;


void setup() {
    // initialise the objects
    Serial.print("Being initialisation")
    Wire.begin(); // basic arduino library to read connections
    delay(1000);
    // set the frequency
    Serial.print("Initialise radio object")
    radio.initialise();
    radio.setFrequency(c_minFreq);
    delay(1000);

    // intialise the lcd
    Serial.print("Initialise lcd object")
    g_lcd.begin();
    g_lcd.backlight();
    g_lcd.clear();

    // initialise the rotary encoder
    Serial.print("Initialise rotary encoder")
    pinMode(c_rotaryEncoderDat, INPUT);
    pinMode(c_rotaryEncoderClk, INPUT);
    Serial.begin(9600); // set baud rate
    lastState = digitalRead(c_rotaryEncoderClk);

    Serial.print("Print welcome message") // TODO: we can do a real message
    g_lcd.setCursor(3, 0);
    g_lcd.print("EAGLE RADIO");
    g_lcd.setCursor(3, 1);
    g_lcd.print(c_minFreq);
    delay(1000);
}

void loop() {
    // set frequency constraints
    float current_frequency;
    current_frequency = constrain(current_frequency, c_minFreq, c_maxFreq);

    // read button states
    int memButton1State = digitalRead(c_memButton1Pin);
    int memButton2State = digitalRead(c_memButton2Pin);

    // memory buttons
    if (memButton1State == HIGH) {
        Serial.print("Button press: memButton1")
        frequencyUpdate(c_minFreq);
    }

    if (memButton2State == HIGH) {
        Serial.print("Button press: memButton2")
        frequencyUpdate(c_maxFreq);
    }

    // rotary encoder
    g_reClkState = digitalRead(c_rotaryEncoderClk);
    g_reDatState = digitalRead(c_rotaryEncoderDat);

    if (g_reClkState != g_reLastState) {
        // pulse occured
        if (g_reDatState != g_reClkState) {
            // clockwise
            if (g_volume < 18) {
                g_volume++;
            }
        } else {
            // anticlockwise
            if (g_volume > 0) {
                g_volume--;
            }
        }
    }
    g_reLastState = g_reClkState;
    g_radio.setVolume(g_volume);
}

void frequencyUpdate(float frequency) {
    // set the frequency
    g_radio.setFrequency(frequency);
    // print to the LCD
    g_lcd.setCursor(3, 1);
    g_lcd.print(frequency);
}
