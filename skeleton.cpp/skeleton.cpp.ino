#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>

// global constants, c_ corresponds to constant
const float c_minFreq = 96.4;
const float c_maxFreq = 107.9;
const float c_memFreq1 = 100.0; // TODO fix
const float c_memFreq2 = 101.0;
const int c_rotaryEncoderDat 3;
const int c_rotaryEncoderClk 2;
const int c_memButton1Pin 8;
const int c_memButton2Pin 9;
const int c_frequButton1Pin 6;
const int c_frequButton2Pin 7;
const int c_lcdDataPath 0x27; // TODO what is this acctually
const int c_lcdLen 16;
const int c_lcdHeight 2;
const int c_maxVolume 18;

// creating the radio and LCD objects, g_ corresponds to global
AR1010 g_radio = AR1010();
LiquidCrystal_I2C g_lcd(c_lcdDataPath, c_lcdHeight, c_lcdLen);
int g_reClkState = 0;
int g_reDatState = 0;
int g_reLastState = 0;
int g_volume = 0;
g_volume = constrain(g_volume, 0, c_maxVolume);

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
    // set variables
    float current_frequency;

    // read button states
    int memButton1State = digitalRead(c_memButton1Pin);
    int memButton2State = digitalRead(c_memButton2Pin);
    int frequButton1State = digitalRead(c_frequButton1Pin);
    int frequButton2State = digitalRead(c_frequButton2Pin);

    // memory buttons
    if (memButton1State == HIGH) {
        Serial.print("Button press: memButton1")
        frequencyUpdate(c_memFreq1);
    }

    if (memButton2State == HIGH) {
        Serial.print("Button press: memButton2")
        frequencyUpdate(c_memFreq2);
    }

    // frequency change buttons
    if (frequButton1State == HIGH) {
        Serial.print("Button press: frequButton1State")
        current_frequency = radio.seek('u');
        if (current_frequency > c_maxFreq) {
            current_frequency = c_minFreq;
        }
        frequencyUpdate(current_frequency);
    }

    if (frequButton2State == HIGH) {
        Serial.print("Button press: frequButton2State")
        current_frequency = radio.seek('d');
        if (current_frequency > c_minFreq) {
            current_frequency = c_maxFreq;
        }
        frequencyUpdate(current_frequency);
    }

    // rotary encoder
    g_reClkState = digitalRead(c_rotaryEncoderClk);
    g_reDatState = digitalRead(c_rotaryEncoderDat);

    if (g_reClkState != g_reLastState) {
        // pulse occured
        if (g_reDatState != g_reClkState) {
            // clockwise
            g_volume++;
        } else {
            // anticlockwise
            g_volume--;
        }
    }
    g_reLastState = g_reClkState;
    g_radio.setVolume(g_volume);
}

void frequencyUpdate(float frequency) {
    // set the frequency
    g_radio.setFrequency(frequency);
    // print to the LCD
    g_lcd.clear();
    g_lcd.setCursor(3, 1); // TODO: test in labs
    g_lcd.print(frequency);
}
