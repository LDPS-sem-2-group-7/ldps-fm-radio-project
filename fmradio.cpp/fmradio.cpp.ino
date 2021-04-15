#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>
#include <string.h>
#include "constants.h"
#include "DS3231.h"
#include "stdlib.h"

// global variables, g_ corresponds to global
int g_reClkState = 0;
int g_reDatState = 0;
int g_reLastState = 0;
int g_reSwState = 0;
int g_volume = 5;
int g_day = 0;
int g_month = 0;
int g_year = 0;
int g_hour = 0;
int g_minute = 0;
int g_freqChangeState = 0;
int g_volChangeState = 0;
bool g_muteState = false;
float g_current_frequency = c_memFreq1;

// global objects, intialised here
AR1010 g_radio;
LiquidCrystal_I2C g_lcd = LiquidCrystal_I2C(c_i2cDataPath, c_lcdHeight, c_lcdLen);
DS3231 g_rtc = DS3231(); // no pins passed

void setup() {
    Serial.print("Being initialisation");
    Wire.begin(); // basic arduino library to read connections

    // set the frequency
    Serial.print("Initialise radio object");
    g_radio = AR1010();
    g_radio.initialise();
    g_radio.setFrequency(c_memFreq1);
    g_radio.seek('u');
    g_radio.setVolume(10);

    // intialise the lcd
    Serial.print("Initialise lcd object");
    g_lcd.begin(c_i2cDataPath, c_lcdHeight, c_lcdLen);
    g_lcd.backlight();

    // intialise the rtc
    Serial.print("Initialise rtc");
    DS3231 g_rtc = DS3231(); // no pins passed
    g_lcd.clear();

    // initialise the rotary encoder
    Serial.print("Initialise rotary encoder");

    pinMode(c_reDat, INPUT); // input pullup used in sims
    pinMode(c_reClk, INPUT);

    attachInterrupt(0, volumeFlag, RISING);

    pinMode(c_reSw, INPUT);
    Serial.begin(9600); // set baud rate


    if (!g_rtc.oscillatorCheck()) {
        Serial.println("RTC not running, set date and time");
        g_rtc.setDate(26);
        g_rtc.setMonth(03);
        g_rtc.setYear(21);
        g_rtc.setHour(10);
        g_rtc.setMinute(30);
        g_rtc.setSecond(00);
    }
}

void loop() {
    // read button states
    int memButton1State = digitalRead(c_memButton1Pin);
    int memButton2State = digitalRead(c_memButton2Pin);
    int frequButton1State = digitalRead(c_frequButton1Pin);
    int frequButton2State = digitalRead(c_frequButton2Pin);
    int g_timeHourButtonState = digitalRead(c_timeHour);
    int g_timeMinButtonState = digitalRead(c_timeMin);
    pinMode(c_memButton1Pin, INPUT_PULLUP);
    pinMode(c_memButton2Pin, INPUT_PULLUP);
    pinMode(c_frequButton1Pin, INPUT_PULLUP);
    pinMode(c_frequButton2Pin, INPUT_PULLUP);
    pinMode(c_timeHour, INPUT_PULLUP);
    pinMode(c_timeMin, INPUT_PULLUP);


    delay(20);

    // volume and freq change state.
    if (g_volChangeState > 0) {
        g_volChangeState--;
    }

    if (g_freqChangeState > 0) {
        g_freqChangeState--;
    }

    // if vol and freq changed in quick succession, keep last change
    if (g_volChangeState && g_freqChangeState) {
        if (g_volChangeState > g_freqChangeState) {
            g_freqChangeState = 0;
        } else {
            g_volChangeState = 0;
        }
    }

    printDisplay(g_current_frequency, g_volChangeState, g_freqChangeState);

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
    //    g_radio.setHardmute(g_muteState); //
    g_radio.setHardmute(false); // TODO: make the mute button work (first)

    // volume control
    /*
    if (g_reClkState != g_reLastState) {
        Serial.print("RE pulse");
        // pulse occured
        if (g_reDatState != g_reClkState) {
            // clockwise
            Serial.print("Clockwise turn");
            g_volume++;
            g_lcd.setCursor(0,0);
            g_lcd.print(g_volume);
        } else {
            // anticlockwise
            Serial.print("Anticlockwise turn");
            g_volume--;
            g_lcd.setCursor(0,0);
            g_lcd.print(g_volume);
        }

    }
*/
    g_reLastState = g_reClkState;
    g_radio.setVolume(g_volume);

    //memory buttons
    if (memButton1State == LOW) {
        g_lcd.setCursor(0, 0);
        g_lcd.print("BUTTON m1");
        Serial.print("Button press: memButton1");
        g_radio.setFrequency(c_memFreq1);
        g_current_frequency = c_memFreq1;
        g_freqChangeState = 50;
        return NULL;
    }

    if (memButton2State == LOW) {
        g_lcd.setCursor(0, 0);
        g_lcd.print("BUTTON m2");
        Serial.print("Button press: memButton2");
        g_radio.setFrequency(c_memFreq2);
        g_current_frequency = c_memFreq2;
        g_freqChangeState = 50;
        return NULL;
    }

    // frequency change buttons
    if (frequButton1State == LOW) {
        g_lcd.setCursor(0, 0);
        g_lcd.print("BUTTON f1");
        Serial.print("Button press: frequButton1State");
        g_current_frequency = g_radio.seek('u');
        if (g_current_frequency > c_maxFreq) {
            g_current_frequency = c_minFreq;
        }
        g_radio.setFrequency(g_current_frequency);
        g_freqChangeState = c_tickDelay;
        return NULL;
    }

    if (frequButton2State == LOW) {
        g_lcd.setCursor(0, 0);
        g_lcd.print("BUTTON f2");
        Serial.print("Button press: frequButton2State");
        g_current_frequency = g_radio.seek('d');
        if (g_current_frequency > c_minFreq) {
            g_current_frequency = c_maxFreq;
        }
        g_radio.setFrequency(g_current_frequency);
        g_freqChangeState = c_tickDelay;
        return NULL;
    }

    if (g_timeHourButtonState == LOW) {
              g_lcd.setCursor(0, 0);

      g_lcd.print("BUTTON t1");
        Serial.print("Button press: g_timeHourButtonState");

        g_hour++;
        if (g_hour == 24) {
            g_hour = 0;
        }

        g_rtc.setHour(g_hour);
        return NULL;
    }

    if (g_timeMinButtonState == LOW) {
              g_lcd.setCursor(0, 0);

      g_lcd.print("BUTTON t2");
        Serial.print("Button press: g_timeMinButtonState");

        g_minute++;
        if (g_minute == 60) {
            g_minute = 0;
        }

        g_rtc.setMinute(g_minute);
        return NULL;
    }
}

void printDisplay(float frequency, int volCount, int freqCount) {
    /*
      |2020-13-45 15:00|
      |█████████       |
    OR
      |97.8HZ - EAGLE R| >>> | ADIO
    */
    bool a = false;
    String dateTime = String(g_rtc.getHour(a, a)) + ":" + String(g_rtc.getMinute()) + '|' + String(g_rtc.getDate()) + '/' + String(g_rtc.getMonth(a)) + '/' + String(g_rtc.getYear());
    //String temp = String(g_rtc.getTemperature()) + "°C ";
    //dateTime += temp;

    // Write the time string
    //g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(dateTime);
    //g_lcd.scrollDisplayRight();

    // Write the frequency string
    g_lcd.setCursor(0, 1);
    //g_lcd.print(temp);
    //    g_lcd.print(frequency);

    // If volume changed, set
    if (volCount) {
        String vStr = volumeString(g_volume);
        g_lcd.setCursor(0, 1);
        // g_lcd.print(name);
    }

    // If frequency changed, and available, display radio station name
    String name = stationName(frequency);
    name += "                ";
    g_lcd.setCursor(0, 1);
    g_lcd.print(name);
}

String volumeString(int volume) {
    String out_string;
    int count = volume * (16 / 18); // sets number of bars to complete

    // create the string
    //out_string = String(count, "█") + String(16 - count, " ");
    out_string = "VOL CHANGED";

    // check if muted
    if (g_muteState) {
        out_string = "MUTED";
    }

    return out_string;
}

String stationName(float freq) {
    String stationName = String(freq);

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

void volumeFlag() {
    g_lcd.setCursor(0, 0);
    g_lcd.print("BUTTON v1");

    if (digitalRead(c_reClk) == digitalRead(c_reDat)) {
        g_volume++;
        if (g_volume > 18) {
            g_volume = 18;
        }
    } else {
        g_volume--;
        if (g_volume < 0) {
            g_volume = 0;
        }
    }

    g_volChangeState = c_tickDelay;
}
