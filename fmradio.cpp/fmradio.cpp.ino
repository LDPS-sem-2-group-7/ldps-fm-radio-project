#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>
#include <string>
#include "constants.h"

// constants moved to constants.h
// global variables, g_ corresponds to global
int g_reClkState = 0;
int g_reDatState = 0;
int g_reLastState = 0;
int g_volume = 0;
int g_day = 0;
int g_month = 0;
int g_year = 0;
int g_hour = 0;
int g_minute = 0;
float g_muteState = false;
//int g_second = 0; removed for clarity and efficiency
g_volume = constrain(g_volume, c_minVolume, c_maxVolume);

void setup() {
    // initialise the objects
    Serial.print("Being initialisation")
    Wire.begin(); // basic arduino library to read connections
    delay(500);

    // set the frequency
    Serial.print("Initialise radio object")
    AR1010 g_radio = AR1010();
    radio.initialise();
    radio.setFrequency(c_minFreq);
    delay(500);

    // intialise the lcd
    Serial.print("Initialise lcd object")
    LiquidCrystal_I2C g_lcd(c_i2cDataPath, c_lcdHeight, c_lcdLen);
    g_lcd.begin();
    g_lcd.backlight();
    g_lcd.clear();

    // intialise the rtc
    Serial.print("Initialise rtc")
    DS3231 g_rtc(c_rtcPinSDA, c_rtcPinSCL);

    // initialise the rotary encoder
    Serial.print("Initialise rotary encoder")
    pinMode(c_reDat, INPUT_PULLUP); // input pullup used in sims
    pinMode(c_reClk, INPUT_PULLUP);
    pinMode(c_reSw, INPUT_PULLUP);
    Serial.begin(9600); // set baud rate
    lastState = digitalRead(c_rotaryEncoderClk);

    // print the welcome message
    Serial.print("Print welcome message") // TODO: we can do a real message
    g_lcd.setCursor(3, 0);
    g_lcd.print("EAGLE RADIO");
    g_lcd.setCursor(3, 1);
    g_lcd.print(c_minFreq);

    if(! g_rtc.isrunning()){
      Serial.println("RTC is not running !");
      rtc.adjust(DateTime(03 26 2021,10:30:00));
    }
    //set the time to the globals.
    DateTime now = rtc.now();
    g_year = now.year();g_month = now.month(); g_day = now.day();
    g_hour = now.hour();g_minute = now.minute();
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
    int timeUpButtonState = digitalRead(c_timeUp);
    int timeDownButtonState = digitalRead(c_timeDown);


    delay(500);
    print_display(frequency);

    //// rotary encoder
    g_reClkState = digitalRead(c_reClk);
    g_reDatState = digitalRead(c_reDat);
    g_reSwState = digitalRead(c_reSw);

    // mute switch
    if (c_reSwState == HIGH) {
        g_muteState = true;
    } else {
        g_muteState = false;
    }
    radio.setHardmute(g_muteState);

    // volume control
    if (g_reClkState != g_reLastState) {
        Serial.print("RE pulse")
        // pulse occured
        if (g_reDatState != g_reClkState) {
            // clockwise
            Serial.print("Clockwise turn")
            g_volume++;
            displayVolume();
        } else {
            // anticlockwise
            Serial.print("Anticlockwise turn")
            g_volume--;
            displayVolume();
        }
    }
    g_reLastState = g_reClkState;
    g_radio.setVolume(g_volume);

    // memory buttons
    if (memButton1State == LOW) {
        Serial.print("Button press: memButton1")
        frequencyUpdate(c_memFreq1);
        continue;
    }

    if (memButton2State == LOW) {
        Serial.print("Button press: memButton2")
        frequencyUpdate(c_memFreq2);
        continue;
    }

    // frequency change buttons
    if (frequButton1State == LOW) {
        Serial.print("Button press: frequButton1State")
        current_frequency = radio.seek('u');
        if (current_frequency > c_maxFreq) {
            current_frequency = c_minFreq;
        }
        frequencyUpdate(current_frequency);
        continue;
    }

    if (frequButton2State == LOW) {
        Serial.print("Button press: frequButton2State")
        current_frequency = radio.seek('d');
        if (current_frequency > c_minFreq) {
            current_frequency = c_maxFreq;
        }
        frequencyUpdate(current_frequency);
        continue;
    }
  if(timeUpButtonState == LOW){
    Serial.print("Button press: timeUpButtonState")
    if(g_hour == 23){
      g_hour == 0;
    }
    if(g_minute ==59){
      g_minute == 0
      g_hour++;
    }else{
      g_minute++;
    }
    now.month() = g_month;
    now.day() = g_day;
    now.year() = g_day;
    now.hour() = g_hour;
    now.minute()  = g_minute;
    now.second() = g_second;
    rtc.now() = now;
    continue;
  }
  if(timeDownButtonState == LOW){
    DateTime now;
    Serial.print("Button press: timeUpButtonState")
    if(g_hour == 0){
      g_hour == 23;
    }
    if(g_minute ==0){
      g_minute == 59
      g_hour--;
    }else{
      g_minute--;
    }
    now.month() = g_month;
    now.day() = g_day;
    now.year() = g_day;
    now.hour() = g_hour;
    now.minute()  = g_minute;
    now.second() = g_second;
    rtc.now() = now;
    continue;
  }
}

void displayVolume() {
    string out_string;
    int count = g_volume * (16 / 18); // sets number of bars to complete

    // create the string
    out_string = string(count, "█") + string(16 - count, " ");

    // check if muted
    if (g_muteState) {
        out_string = "MUTED";
    }

    // print to the lcd
    g_lcd.clear();
    g_lcd.setCursor(3, 1);
    g_lcd.print(out_string);
}

void print_display(float frequency) {//default LCD output.
    /*
      |2020-13-45 15:00| >>> | 23 °
      |█████████       |
    OR
      |97.8HZ - EAGLE R| >>> | ADIO
    */

    string date = to_string(g_year) + '/' + to_string(g_month) + '/' + to_string(g_day);
    string time = to_string(g_hour) + ':' + to_string(g_minute);
    string temp = to_string(g_rtc.getTemp()) + 'C';
    string dateTimeTemp = date + time + temp;
    g_lcd.clear();
    g_lcd.setCursor(3, 0);
    g_lcd.print(dateTime);
    g_lcd.rightToLeft();
    g_lcd.setCursor(3, 1);
    g_lcd.print(frequency);

    // TODO: this overwrites the time, do we want to add a delay?
    string name = station_name(frequency);
    g_lcd.setCursor(3, 0);
    g_lcd.print(name);
}

string station_name(float freq) {
    // get the station names
    string station_name = "";

    if (freq == 105.8) {
        station_name = "Absolute Radio";
    } else if (97.1 <= freq && freq <= 99.7) {
        station_name = "BBC Radio 1";
    } else if (88.1 <= freq && freq <= 90.2) {
        station_name = "BBC Radio 2";
    } else if ((90.3 <= freq && freq <= 92.4) || freq == 92.6) {
        station_name = "BBC Radio 3";
    } else if ((92.5 <= freq && freq <= 96.1) || (103.5 <= freq && freq <= 104.9)) {
        station_name = "BBC Radio 4";
    } else if (99.9 <= freq && freq <= 101.9) {
        station_name = "Classic FM";
    } else if (freq == 100.0) {
        station_name = "Kiss";
    } else if (freq == 97.3) {
        station_name = "LBC";
    } else if (freq == 105.4) {
        station_name = "Magic";
    }

    return station_name;
}



void frequencyUpdate(float frequency) {
    // set the frequency
    g_radio.setFrequency(frequency);
    // print to the LCD
    g_lcd.clear();
    g_lcd.setCursor(3, 1); // TODO: test in labs
    g_lcd.print(frequency);
}
