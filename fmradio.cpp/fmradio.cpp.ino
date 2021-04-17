#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>
#include <string.h>
#include "constants.h"
#include "byteArrays.h"
#include "DS3231.h"
#include "stdlib.h"

// global variables, whose state must be preserved across loops
volatile int g_volume = 10;
volatile byte g_volUpFlag = 0;
volatile byte g_volDownFlag = 0;
int g_volChangeState = 0;
bool g_muteState = false;

// global objects, intialised here
AR1010 g_radio;
LiquidCrystal_I2C g_lcd = LiquidCrystal_I2C(c_i2cDataPath, c_lcdHeight, c_lcdLen);
DS3231 g_rtc = DS3231(); // no pins passed

void setup() {
    Wire.begin(); // basic arduino library to read connections

    // set the frequency
    g_radio = AR1010();
    g_radio.initialise();
    g_radio.setFrequency(c_memFreq1);
    g_radio.setVolume(g_volume);

    // intialise the lcd
    g_lcd.begin(c_i2cDataPath, c_lcdHeight, c_lcdLen);
    g_lcd.createChar(0, zero);
    g_lcd.createChar(1, one);
    g_lcd.createChar(2, two);
    g_lcd.createChar(3, three);
    g_lcd.createChar(4, four);
    g_lcd.createChar(5, five);
    g_lcd.backlight();

    // intialise the rtc
    DS3231 g_rtc = DS3231(); // no pins passed
    g_lcd.clear();

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

    attachInterrupt(0, volumeFlag, FALLING);

    Serial.begin(9600);

    if (!g_rtc.oscillatorCheck()) {
        setDefaultRTC();
    }
}

void setDefaultRTC() {
    g_rtc.setDate(26);
    g_rtc.setMonth(03);
    g_rtc.setYear(21);
    g_rtc.setHour(10);
    g_rtc.setMinute(30);
    g_rtc.setSecond(00);
}

void loop() {
    // read button states
    int memButton1State = digitalRead(c_memButton1Pin);
    int memButton2State = digitalRead(c_memButton2Pin);
    int frequButton1State = digitalRead(c_frequButton1Pin);
    int frequButton2State = digitalRead(c_frequButton2Pin);
    int timeHourButtonState = digitalRead(c_timeHour);
    int timeMinButtonState = digitalRead(c_timeMin);
    int reSwState = digitalRead(c_reSw);

    printDisplay(g_radio.frequency(), g_volChangeState);
    delay(20); // avoids accidental double presses

    // volume and freq change state.
    // TODO: test if constrain(g_volChangeState--, 0, c_tickDelay) works
    if (g_volChangeState > 0) {
        g_volChangeState--;
    }

    if (g_volUpFlag || g_volDownFlag) {
        volButtons()
    }

    if (reSwState == LOW) {
        buttonMute();
    } else if (memButton1State == LOW) {
        g_radio.setFrequency(c_memFreq1);
    } else if (memButton2State == LOW) {
        g_radio.setFrequency(c_memFreq2);
    } else if (frequButton1State == LOW) {
        buttonFreqUp();
    } else if (frequButton2State == LOW) {
        buttonFreqDown();
    } else if (timeHourButtonState == LOW) {
        buttonTimeHour();
    } else if (timeMinButtonState == LOW) {
        buttonTimeMin();
    }
}

void volButtons() {
    if (g_volUpFlag) {
        g_volume++;
        g_volUpFlag = 0;
    } else if (g_volDownFlag) {
        g_volume--;
        g_volDownFlag = 0;
    }
    constrain(g_volume, 0, 18);
    g_radio.setVolume(g_volume);
}

void buttonMute() {
    g_muteState = !g_muteState;
    g_volChangeState = c_tickDelay;
    g_radio.setHardmute(g_muteState);
    delay(50);
}

void buttonFreqUp() {
    float foundFreq;
    if (g_radio.frequency() == c_maxFreqKnown){
        foundFreq = c_minFreqKnown;
    } else {
        float foundFreq = g_radio.seek('u');
    }

    g_radio.setFrequency(foundFreq);
}

void buttonFreqDown() {
    float foundFreq;
    if (g_radio.frequency() == c_minFreqKnown){
        foundFreq = c_maxFreqKnown;
    } else {
        float foundFreq = g_radio.seek('d');
    }

    g_radio.setFrequency(foundFreq);
}

void buttonTimeHour() {
    int hourToSet = g_rtc.getHour();
    hourToSet++;
    if (hourToSet == 24) {
        hourToSet = 0;
    }
    g_rtc.setHour(hourToSet);
}

void buttonTimeMin() {
    int minToSet = g_rtc.getMinute();
    hourToSet++;
    if (hourToSet == 60) {
        hourToSet = 0;
    }
    g_rtc.setMinute(hourToSet);
}

String PadTwo(String input) {
    String output;
    if (input.length() == 1) {
        output = "0" + input;
    } else {
        output = input;
    }
    return output;
}

void printDisplay(float frequency, int volCount, int freqCount) {
    // if we're printing the volume change we don't need anything else
    if (g_volChangeState > 0) {
        printVolume();
    } else {
        printTimeAndFreq(frequency, volCount, freqCount);
    }
}

Void printVolume() {
    g_lcd.setCursor(0, 0);
    g_lcd.print(padRight("Volume:"));

    if (g_muteState) {
        g_lcd.setCursor(0, 1);
        g_lcd.print(padRight("MUTED"));
        return;
    }

    String vStr = volumeString(g_volume);
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
    String stationName = stationName(frequency);

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

String stationName(float freq) {
    String stationName = "";

    if (freq == 105.8) {
        stationName = "Absolute Radio";
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
    } else {
        stationName = String(freq) + "Hz";
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

    g_volChangeState = c_tickDelay;
}
