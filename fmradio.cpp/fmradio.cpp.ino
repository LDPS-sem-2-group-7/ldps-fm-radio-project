#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>
#include <string.h>
#include "constants.h"
#include "DS3231.h"

#include "stdlib.h"

// constants moved to constants.h
// global variables, g_ corresponds to global
int g_reClkState = 0;
int g_reDatState = 0;
int g_reLastState = 0;
int g_reSwState = 0;
int g_volume = 0;
int g_day = 0;
int g_month = 0;
int g_year = 0;
int g_hour = 0;
int g_minute = 0;
float g_muteState = false;
//int g_second = 0; removed for clarity and efficiency
//////g_volume = constrain(g_volume, c_minVolume, c_maxVolume); // todo

AR1010 g_radio;
LiquidCrystal_I2C g_lcd = LiquidCrystal_I2C(c_i2cDataPath, c_lcdHeight, c_lcdLen);;

void setup() {
    // initialise the objects
    Serial.print("Being initialisation");
    Wire.begin(); // basic arduino library to read connections
    delay(500);

    // set the frequency
    Serial.print("Initialise radio object");
    g_radio = AR1010();
    g_radio.initialise();
    g_radio.setFrequency(c_memFreq1);
    delay(500);

    // intialise the lcd
    Serial.print("Initialise lcd object");
    //    g_lcd = LiquidCrystal_I2C(c_i2cDataPath, c_lcdHeight, c_lcdLen);
    g_lcd.begin(c_i2cDataPath, c_lcdHeight, c_lcdLen);
    g_lcd.backlight();
    g_lcd.clear();

    // intialise the rtc
    Serial.print("Initialise rtc");
    DS3231 g_rtc = DS3231(); // no pins passed

    // initialise the rotary encoder
    Serial.print("Initialise rotary encoder");
    pinMode(c_reDat, INPUT_PULLUP); // input pullup used in sims
    pinMode(c_reClk, INPUT_PULLUP);
    pinMode(c_reSw, INPUT_PULLUP);
    Serial.begin(9600); // set baud rate
    g_reLastState = digitalRead(c_reClk);

    // print the welcome message
    Serial.print("Print welcome message"); // TODO: we can do a real message
    g_lcd.setCursor(3, 0);
    g_lcd.print("EAGLE RADIO");
    g_lcd.setCursor(3, 1);
    g_lcd.print(c_memFreq1);

    if (!g_rtc.oscillatorCheck()) {
        Serial.println("RTC is not running !");
        //        g_rtc.adjust(DateTime(03 26 2021, 10 : 30 : 00));
    }

    //set the time to the globals.
    g_year = g_rtc.getYear();
    //    g_month = g_rtc.getMonth(true);
    g_month = 3;
    g_day = g_rtc.getDate();
    //    g_hour = g_rtc.getHour();
    g_hour = 11;
    g_minute = g_rtc.getMinute();
    delay(500);
}

void loop() {
    // set variables
    float current_frequency = c_minFreq;

    // read button states
    int memButton1State = digitalRead(c_memButton1Pin);
    int memButton2State = digitalRead(c_memButton2Pin);
    int frequButton1State = digitalRead(c_frequButton1Pin);
    int frequButton2State = digitalRead(c_frequButton2Pin);
    int g_timeHourButtonState = digitalRead(c_timeHour);
    int g_timeMinButtonState = digitalRead(c_timeMin);


    delay(500);
    printDisplay(current_frequency);

    //// rotary encoder
    g_reClkState = digitalRead(c_reClk);
    g_reDatState = digitalRead(c_reDat);
    g_reSwState = digitalRead(c_reSw);

    // mute switch
    if (g_reSwState == HIGH) {
        g_muteState = true;
    } else {
        g_muteState = false;
    }
    g_radio.setHardmute(g_muteState);

    // volume control
    if (g_reClkState != g_reLastState) {
        Serial.print("RE pulse");
        // pulse occured
        if (g_reDatState != g_reClkState) {
            // clockwise
            Serial.print("Clockwise turn");
            g_volume++;
            displayVolume();
        } else {
            // anticlockwise
            Serial.print("Anticlockwise turn");
            g_volume--;
            displayVolume();
        }
    }
    g_reLastState = g_reClkState;
    g_radio.setVolume(g_volume);

    // memory buttons
    if (memButton1State == LOW) {
        Serial.print("Button press: memButton1");
        frequencyUpdate(c_memFreq1);
        return NULL;
    }

    if (memButton2State == LOW) {
        Serial.print("Button press: memButton2");
        frequencyUpdate(c_memFreq2);
        return NULL;
    }

    // frequency change buttons
    if (frequButton1State == LOW) {
        Serial.print("Button press: frequButton1State");
        current_frequency = g_radio.seek('u');
        if (current_frequency > c_maxFreq) {
            current_frequency = c_minFreq;
        }
        frequencyUpdate(current_frequency);
        return NULL;
    }

    if (frequButton2State == LOW) {
        Serial.print("Button press: frequButton2State");
        current_frequency = g_radio.seek('d');
        if (current_frequency > c_minFreq) {
            current_frequency = c_maxFreq;
        }
        frequencyUpdate(current_frequency);
        return NULL;
    }

    //    if (g_timeHourButtonState == LOW) {
    //        DateTime now; // temp object, copied to global object
    //        Serial.print("Button press: g_timeHourButtonState");
    //
    //        g_hour++;
    //        if (g_hour == 24) {
    //            g_hour = 0;
    //        }
    //
    //        now.month() = g_month;
    //        now.day() = g_day;
    //        now.year() = g_day;
    //        now.hour() = g_hour;
    //        now.minute() = g_minute;
    //        now.second() = g_second;
    //        g_rtc.now() = now;
    //        return NULL;
    //    }
    //
    //    if (g_timeMinButtonState == LOW) {
    //        DateTime now;
    //        Serial.print("Button press: g_timeMinButtonState")
    //
    //        g_minute++;
    //        if (g_minute == 60) {
    //            g_minute = 0;
    //        }
    //
    //        now.month() = g_month;
    //        now.day() = g_day;
    //        now.year() = g_day;
    //        now.hour() = g_hour;
    //        now.minute() = g_minute;
    //        now.second() = g_second;
    //        g_rtc.now() = now;
    //        return NULL;
    //    }
}

void displayVolume() {
    String out_string;
    int count = g_volume * (16 / 18); // sets number of bars to complete

    // create the string
    //    out_string = String(count, "█") + String(16 - count, " ");
    out_string = "temp";

    // check if muted
    if (g_muteState) {
        out_string = "MUTED";
    }

    // print to the lcd
    g_lcd.clear();
    g_lcd.setCursor(3, 1);
    g_lcd.print(out_string);
}

void printDisplay(float frequency) {//default LCD output.
    /*
      |2020-13-45 15:00| >>> | 23 °
      |█████████       |
    OR
      |97.8HZ - EAGLE R| >>> | ADIO
    */
    //
    //    String date = to_string(g_year) + '/' + to_string(g_month) + '/' + to_string(g_day);
    //    String time = to_string(g_hour) + ':' + to_string(g_minute);
    //    String temp = to_string(g_rtc.getTemp()) + 'C';
    //    String dateTimeTemp = date + time + temp;
    String dateTimeTemp = "TEMPORARY"; // TODO: fix
    g_lcd.clear();
    g_lcd.setCursor(3, 0);
    g_lcd.print(dateTimeTemp);
    g_lcd.rightToLeft();
    g_lcd.setCursor(3, 1);
    g_lcd.print(frequency);

    // TODO: this overwrites the time, do we want to add a delay?
    String name = stationName(frequency);
    g_lcd.setCursor(3, 0);
    g_lcd.print(name);
}

String stationName(float freq) {
    // get the station names
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
    }

    return stationName;
}

void frequencyUpdate(float frequency) {
    // set the frequency
    g_radio.setFrequency(frequency);
    // print to the LCD
    g_lcd.clear();
    g_lcd.setCursor(3, 1); // TODO: test in labs
    g_lcd.print(frequency);
}
