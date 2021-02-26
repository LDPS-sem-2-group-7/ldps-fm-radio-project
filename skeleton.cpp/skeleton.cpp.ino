#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>


AR1010 radio = AR1010();
float frequency = 0;
float startFreq = 96.4
    int powerRadio = 0;//where 0 = off
//we should declare component variables here.


void setup() {
    // put your setup code here, to run once:
    Wire.begin();
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    radio.initialise();
    delay(1000);
    radio.setFrequency(startFreq);//radio is an instanation of the radio module in the radio.h header file
}

void loop() {
    // put your main code here, to run repeatedly:
}

void radio() {
    //when seek button pressed.
    float frequency;
    float newFrequency;
    while (button == "low") {
        if (frequency = maxFreq) {
            frequency = startFrequency;
        }
        newFrequency = frequency + 0.1;
        setFrequency(newFrequency);
        frequency = newFrequency;
        // display newFrequency
        delay(20);
    }
    //when mem button pressed
    while (membutton1 == "low") {
        frequency = 105.4;
        setFrequency(frequency);
    }
}
