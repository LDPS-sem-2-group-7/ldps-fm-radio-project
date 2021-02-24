//#include <Wire.h> //a header file which contians all wires in arduino.
//#include <radio.h> // need the headerfile for the radio chip still
//#include rtc Real time clock

int main(){
    float frequency = 0;
    float startFreq = 96.4
    radio.setFrequency(startFreq);//radio is an instanation of the radio module in the radio.h header file

}
//#define some stations held in memory.

void frequencySwitch(float freq){
    //use radio frequency to restrict the frequency between a short range e.g 72.0 and 111.0
    //lcd print "Station: " at 0,0
    //lcd print frequency: at 0,1
    
    

}
void radio(){
    //when seek button pressed.
    while(button == "low"){
        frequency += 0.1;
        frequencySwitch(frequency);
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