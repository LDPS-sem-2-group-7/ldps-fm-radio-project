#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>

// creating the radio and LCD objects
AR1010 g_radio = AR1010();
LiquidCrystal_I2C g_lcd(0x27, 16, 2);//g_ corresponds to global.

// global variables
const float c_minFreq = 96.4;//c_ corresponds to constant.
const float c_maxFreq = 107.9;

void setup() {
    // initialise the objects
    Wire.begin();
    g_lcd.begin();
    radio.initialise();
    delay(1000);
    // set the frequency
    radio.setFrequency(c_minFreq);
    // setup the glcd
    g_lcd.backlight();
    g_lcd.clear();
    g_lcd.setCursor(3, 0);
    g_lcd.print("EAGLE RADIO");
    g_lcd.setCursor(3, 1);
    g_lcd.print(c_minFreq);

    //Serial.begin(9600); dont think we need this. Meant for clock
}

void loop() {
    // set frequency constraints
    float frequency;
    frequency = constrain(frequency, c_minFreq, c_maxFreq);

    // read button states
    int memButton1State = digitalRead(MEM BUTTON 1 PIN);
    int memButton2State = digitalRead(MEM BUTTON 2 PIN);

    // memory buttons
    if (memButton1State == HIGH) {
        frequencyUpdate(c_minFreq);
    }

    if (memButton2State == HIGH) {
        frequencyUpdate(c_maxFreq);
    }


    // while (digitalRead(freq up btn)) {//remove while loops since it is a loop() function.
    //     if (digitalRead(freq up)) {
    //         while (digitalRead(freq down btn)) {
    //             if (frequency > c_maxFreq) {
    //                 frequency = c_minFrequency;
    //             }
    //             frequency = radio.seek('u');
    //             frequencyUpdate(frequency);
    //             // display newFrequency
    //             delay(20);
    //         }
    //     } else if (digitalRead(freq down)) {    //not sure what button is this yet
    //         while (downButton == "low") {
    //             if (frequency < c_minFreq) {
    //                 frequency = c_maxFreq;
    //             }
    //             frequency = radio.seek('d');
    //             frequencyUpdate(frequency);
    //             // display newFrequency
    //             delay(20);
    //         }
    //     } else if (digitalRead(mem1)) {
    //         while (digitalRead(freq up btn)) {
    //             frequency = 96.4;
    //             frequencyUpdate(frequency);
    //         }
    //     } else if (digitalRead(mem2)) {
    //         while (digitalRead(freq up btn)) {
    //             frequency = 100.0;
    //             frequencyUpdate(frequency);
    //         }
    //     }
    // }
}

void frequencyUpdate(float frequency) {
    // set the frequency
    g_radio.setFrequency(frequency);
    // print to the LCD
    g_lcd.setCursor(3, 1);
    g_lcd.print(frequency);
}
