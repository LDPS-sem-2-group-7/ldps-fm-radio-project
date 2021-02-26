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

void frequencySwitch(){
    //use radio frequency to restrict the frequency between a short range e.g 72.0 and 111.0
    //lcd print "Station: " at 0,0
    //lcd print frequency: at 0,1
    frequency = constrain(frequency, 87.7,107.9);
    
}

void radio(){
    //when seek button pressed.
    while(button == "low"){
        frequency += 0.1;
        frequencySwitch(frequency);// potentially can use seek function in the ar1010 file, freqswitch is best for memory stored stations.
        delay(20);
    }
    if(maxFreq == frequency){
        frequency = startFrequency
        frequencySwitch(startFrequency);
        delay(20);
    }
    //when mem button pressed
    while(membutton1 == "low"){
        frequency = 105.4
        frequencySwitch(frequency);
    }
}
