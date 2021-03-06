#include <Wire.h>
#include <string.h>
#include "ar1010lib.h"
#include "byteArrays.h"
#include "constants.h"
#include "DS3231.h"
#include "LiquidCrystal_I2C.h"

// global variables, whose state must be preserved across loops
volatile int g_volume = c_defaultVolume;
volatile byte g_volUpFlag = 0;
volatile byte g_volDownFlag = 0;
int g_volChangeTick = 0;
int g_freqChangeTick = c_tickDelay; // shows raw freq on startup
bool g_muteState = false;

// global objects, intialised here
AR1010 g_radio = AR1010();
LiquidCrystal_I2C g_lcd = LiquidCrystal_I2C(c_i2cAddress, c_lcdHeight, c_lcdLen);
DS3231 g_rtc = DS3231();

void setup() {
    Wire.begin(); // basic arduino library to read connections
    
    // set the frequency
    g_radio.initialise();
    g_radio.setFrequency(c_memFreq1 * 10);
    g_radio.setVolume(g_volume);

    // intialise the lcd
    g_lcd.begin(c_i2cAddress, c_lcdHeight, c_lcdLen);
    g_lcd.createChar(0, zero);
    g_lcd.createChar(1, one);
    g_lcd.createChar(2, two);
    g_lcd.createChar(3, three);
    g_lcd.createChar(4, four);
    g_lcd.createChar(5, five);
    g_lcd.backlight();

    // intialise the rtc
    DS3231 g_rtc = DS3231(); // no pins passed

    // set the pin modes for all used pins
    pinMode(c_reDat, INPUT);
    pinMode(c_reClk, INPUT);
    pinMode(c_reSw, INPUT);
    pinMode(c_memButton1Pin, INPUT_PULLUP);
    pinMode(c_memButton2Pin, INPUT_PULLUP);
    pinMode(c_frequButton1Pin, INPUT_PULLUP);
    pinMode(c_frequButton2Pin, INPUT_PULLUP);
    pinMode(c_timeHour, INPUT_PULLUP);
    pinMode(c_timeMin, INPUT_PULLUP);

    // attach the interupt to pin 2 and to the volumeFlag ISR
    attachInterrupt(0, volumeFlag, FALLING);

    // if the RTC has not been used yet, set the default RTC parameters
    if (!g_rtc.oscillatorCheck()) {
        setDefaultRTC();
    }

   buttonMem1(); //TODO: this fixes static but hwy
}

void setDefaultRTC() {
    /*
     * This is run whenever the RTC has not been initialised. The date _can't_
     * be adjusted after it's been set, so it needs to be hard coded. the time
     * can be adjusted.
     */
    g_rtc.setDate(06);
    g_rtc.setMonth(05);
    g_rtc.setYear(21);
    g_rtc.setHour(15);
    g_rtc.setMinute(54);
    g_rtc.setSecond(00);
}

void loop() {
    bool buttonPressed = true; //overwwrites vol ticker if btn pressed

    // read button states
    int memButton1State = digitalRead(c_memButton1Pin);
    int memButton2State = digitalRead(c_memButton2Pin);
    int frequButton1State = digitalRead(c_frequButton1Pin);
    int frequButton2State = digitalRead(c_frequButton2Pin);
    int timeHourButtonState = digitalRead(c_timeHour);
    int timeMinButtonState = digitalRead(c_timeMin);
    int reSwState = digitalRead(c_reSw);

    printDisplay(g_radio.frequency() / 10.0, g_volChangeTick, g_freqChangeTick);
    delay(20); // avoids accidental double presses

    if (g_volUpFlag || g_volDownFlag) {
      // seperate because if part of main if/then we reset ticker to 0
        volButtons();
    }

    if (reSwState == LOW) {
        buttonMute();
    } else if (memButton1State == LOW) {
        buttonMem1();
    } else if (memButton2State == LOW) {
        buttonMem2();
    } else if (frequButton1State == LOW) {
        buttonFreqUp();
    } else if (frequButton2State == LOW) {
        buttonFreqDown();
    } else if (timeHourButtonState == LOW) {
        buttonTimeHour();
    } else if (timeMinButtonState == LOW) {
        buttonTimeMin();
    } else {
        buttonPressed = false;
    }

    if (buttonPressed) {
        // reset vol ticker on button press
        g_volChangeTick = 0;
    } else {
        // decrement/constrain vol ticker if no button is pressed
        g_volChangeTick--;
        g_volChangeTick = constrain(g_volChangeTick, 0, c_tickDelay);
    }
    
    g_freqChangeTick--;
    g_freqChangeTick = constrain(g_freqChangeTick, 0, c_tickDelay);

}

void volButtons() {
    /*
     * This function handles both increases and decreases in volumes. When
     * called it reads the volume flags (set in the ISR when an interupt is
     * detected. The volume is constrained into the limits.
     */
    if (g_volUpFlag) {
        g_volume++;
        g_volUpFlag = 0;
    } else if (g_volDownFlag) {
        g_volume--;
        g_volDownFlag = 0;
    }
    g_volume = constrain(g_volume, c_minVolume, c_maxVolume);
    g_radio.setVolume(g_volume);
    g_volChangeTick = c_tickDelay;
}

void buttonMute() {
    g_muteState = !g_muteState;
    g_radio.setHardmute(g_muteState);
    delay(50); // the mute button double clicks a lot
    g_volChangeTick = c_tickDelay;
}

void buttonMem1() {
    g_radio.setFrequency(c_memFreq1 * 10);
    g_freqChangeTick = c_tickDelay;
}

void buttonMem2() {
    g_radio.setFrequency(c_memFreq2 * 10);
    g_freqChangeTick = c_tickDelay;
}

void buttonFreqUp() {
    /*
     * Increases the volume and rolls to the minimum when required to do so.
     */
     g_radio.setFrequency(g_radio.seek('d'));
     g_freqChangeTick = c_tickDelay;
}

void buttonFreqDown() {
    /*
     * Decreases the volume and rolls to the maximum when required to do so.
     */
     g_radio.setFrequency(g_radio.seek('u'));
     g_freqChangeTick  = c_tickDelay;
}

void buttonTimeHour() {
    bool falseVar = false;
    int hourToSet = g_rtc.getHour(falseVar, falseVar);
    hourToSet++;
    if (hourToSet == 24) {
        hourToSet = 0;
    }
    g_rtc.setHour(hourToSet);
}

void buttonTimeMin() {
    int minToSet = g_rtc.getMinute();
    minToSet++;
    if (minToSet == 60) {
        minToSet = 0;
    }
    g_rtc.setMinute(minToSet);
}

String PadTwo(String input) {
    /*
     * Expects an input of either 1 or 2 numbers. pads the input with an extra
     * 0 if it's only 1 char. For example, if the input is '3' the output is
     * '03'
     */
    String output;
    if (input.length() == 1) {
        output = "0" + input;
    } else {
        output = input;
    }
    return output;
}

void printDisplay(float frequency, int volCount, int freqCount) {
    /*
     * Calls the appropriate display function
     */
    // if we're printing the volume change we don't need anything else
    if (g_volChangeTick > 0) {
        printVolume();
    } else {
        printTimeAndFreq(frequency, volCount, freqCount);
    }
}

void printVolume() {
    g_lcd.setCursor(0, 0);
    g_lcd.print(padRight("Volume:"));

    if (g_muteState) {
        g_lcd.setCursor(0, 1);
        g_lcd.print(padRight("MUTED"));
        return;
    }

    g_lcd.setCursor(0, 1);
    double factor = c_lcdLen / 80.0; // 80 is c_lcdLen * the 5 blocks
    int percent = (g_volume + 1) / factor;
    int number = percent / 5;
    int remainder = percent % 5;

    for (int i = 0; i < 17; i++) {
        if (number < 0) {
            break;
        }
        if (i < number) {
            g_lcd.setCursor(i - 1, 1);
            g_lcd.write(5);
        } else {
            g_lcd.setCursor(i - 1, 1);
            g_lcd.write(0);
        }
    }

    g_lcd.setCursor(number, 1);
    g_lcd.write(remainder);
}

String getDateTimeStr() {
    /*Returns a datetime string formatted to fill the entire line*/
    bool falseVar = false; // we need to pass parameters by reference
    String dateTime = "";
    dateTime += PadTwo(String(g_rtc.getHour(falseVar, falseVar)));
    dateTime += ":";
    dateTime += PadTwo(String(g_rtc.getMinute()));
    dateTime += '|';
    dateTime += PadTwo(String(g_rtc.getDate()));
    dateTime += '/';
    dateTime += PadTwo(String(g_rtc.getMonth(falseVar)));
    dateTime += "/20";
    dateTime += String(g_rtc.getYear());
    return dateTime;
}

String getTempStr() {
    String temperature;
    temperature = String(g_rtc.getTemperature());
    temperature.remove(temperature.length() - 3); // remove decimals
    temperature += "C";
    return temperature;
}

void printTimeAndFreq(float frequency, int volCount, int freqCount) {
    // get the components for the display
    String dateTime = getDateTimeStr();
    String temperature = getTempStr();
    String stationName = getStationName(frequency, freqCount);

    // work out the string for line 2
    int line2padLen = (c_lcdLen - 3) - stationName.length();
    String line2pad = "";
    for (int i = 0; i < line2padLen; i++) {
        line2pad += " ";
    }

    String line2 = stationName + line2pad + temperature;

    //  print to the LCD
    g_lcd.setCursor(0, 0);
    g_lcd.print(dateTime);
    g_lcd.setCursor(0, 1);
    g_lcd.print(line2);
}

String padRight(String input) {
    /* Pads the right part of the input string with spaces, and returns it.

    When printing to the LCD, the cursor is reset and text is written from that
    point on. If text has already been printed at position n and the new text
    being written does not extend to position n (i.e. it is shorter), the
    previous text will remain on the display. One option is to clear the display
    every time we print the text, but this causes flickering. Instead we can pad
    the text with spaces and force an overwrite of the previous text.

    Calling this function with an empty input will return a string of just 16
    spaces.
    */
    return input + "                "; // 16 spaces as that's the max
}

String getStationName(float freq, int freqCount) {
    String stationName = String(freq) + "Hz";;

    if (freqCount>0) {
        return stationName;
    }
    
    if (freq == 105.8) {
        stationName = "Absolute";
    } else if (97.1 <= freq && freq <= 99.7) {
        stationName = "BBC Radio 1";
    } else if (88.1 <= freq && freq <= 90.2) {
        stationName = "BBC Radio 2";
    } else if ((90.3 <= freq && freq <= 92.4) || freq == 92.6) {
        stationName = "BBC Radio 3";
    } else if ((92.5 <= freq && freq <= 96.1) || (103.5 <= freq && freq <= 104.9)) {
        stationName = "BBC Radio 4";
    } else if (99.9 <= freq && freq <= 101.9) {
        stationName = "Classic FM";
    } else if (freq == 100.0) {
        stationName = "Kiss";
    } else if (freq == 97.3) {
        stationName = "LBC";
    } else if (freq == 105.4) {
        stationName = "Magic";
    } else if (freq == 96.4) {
        stationName = "Eagle Radio";
    }

    return stationName;
}

void volumeFlag() {
    /* This is the ISR attached to the rotary encoder interupt*/
    if (digitalRead(c_reClk) == digitalRead(c_reDat)) {
        g_volDownFlag = 1;
    } else {
        g_volUpFlag = 1;
    }

    g_volChangeTick = c_tickDelay;
}
