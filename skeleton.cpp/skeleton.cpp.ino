#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>


AR1010 radio = AR1010();
float frequency = 0;
float startFreq = 96.4;
int powerRadio = 0;//where 0 = off
//we should declare component variables here.
LiquidCrystal_I2C lcd(0x27, 16, 2);//not sure about I2C address.

void setup() {
    // put your setup code here, to run once:
    Wire.begin();
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    radio.initialise();
    delay(1000);
    //the button pin pullup stuff
    radio.setFrequency(startFreq); //radio is an instanation of the radio module in the radio.h header file
    // set constraiaint
    lcd.setCursor(3, 0);//pixelx and pixely are placeholders for actual int pixel values when we know what we want.
    lcd.print("EAGLE RADIO");
    lcd.setCursor();
}

void radio() {
    // when seek button pressed.
    frequency = constrain(frequency, 87.7, 107.9);
    while (upButton == "low") {
        if (frequency > maxFreq) {
            frequency = startFrequency;
        }
        frequency = radio.seek('u');
        // display newFrequency
        delay(20);
    }

    //when mem button pressed
    while (membutton1 == "low") {
        frequency = 105.4;
        setFrequency(frequency);
    }
}
