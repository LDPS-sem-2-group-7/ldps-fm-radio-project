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
    frequency = startFreq;
    radio.setFrequency(frequency); //radio is an instanation of the radio module in the radio.h header file
    // set constraiaint
    lcd.setCursor(3, 0);//pixelx and pixely are placeholders for actual int pixel values when we know what we want.
    lcd.print("EAGLE RADIO");
    lcd.setCursor(3, 1);
    lcd.print(frequency);
    //Serial.begin(9600); dont think we need this. Meant for clock
}
void loop() {
    float minFreq = 87.7;
    float maxFreq = 107.9;
    frequency = constrain(frequency, minFreq, maxFreq);

    while (digitalRead(freq up btn)) {
        if (digitalRead(freq up)) {
            while (digitalRead(freq down btn)) {
                if (frequency > maxFreq) {
                    frequency = minFrequency;
                }
                frequency = radio.seek('u');
                frequencyUpdate(frequency);
                // display newFrequency
                delay(20);
            }
        } else if (digitalRead(freq down)) {    //not sure what button is this yet
            while (downButton == "low") {
                if (frequency < minFreq) {
                    frequency = maxFreq;
                }
                frequency = radio.seek('d');
                frequencyUpdate(frequency);
                // display newFrequency
                delay(20);
            }
        } else if (digitalRead(mem1)) {
            while (digitalRead(freq up btn)) {
                frequency = 96.4;
                frequencyUpdate(frequency);
            }
        } else if (digitalRead(mem2)) {
            while (digitalRead(freq up btn)) {
              frequency = 100.0;
              frequencyUpdate(frequency);
            }
        }
    }
}

void frequencyUpdate(float frequency) {
    // set the frequency
    setFrequency(frequency);
    // print to the LCD
    lcd.setCursor(3, 1);
    lcd.print(frequency);
}
