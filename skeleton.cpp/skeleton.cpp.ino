#include "ar1010lib.h"
#include "LiquidCrystal_I2C.h"
#include <Wire.h>
#include <string>


// global constants, c_ corresponds to constant
const int c_i2cDataPath 0x27;
const float c_minFreq = 96.4;
const float c_maxFreq = 107.9;
const float c_memFreq1 = 100.0; // TODO fix
const float c_memFreq2 = 101.0;
const int c_reDat = 3;
const int c_reClk = 2;
const int c_reSw = 4;
const int c_memButton1Pin 8;//TODO:header file for the constants
const int c_memButton2Pin 9;
const int c_frequButton1Pin 6;
const int c_frequButton2Pin 7;
const int c_rtcPinSDA TODO;///////////
const int c_rtcPinSCL TODO;
const int c_lcdLen 16;
const int c_lcdHeight 2;
const int c_minVolume 0;
const int c_maxVolume 18;

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
    pinMode(c_rotaryEncoderDat, INPUT_PULLUP); // input pullup used in sims
    pinMode(c_rotaryEncoderClk, INPUT_PULLUP);
    pinMode(c_ro)
    Serial.begin(9600); // set baud rate
    lastState = digitalRead(c_rotaryEncoderClk);

    // print the welcome message
    Serial.print("Print welcome message") // TODO: we can do a real message
    g_lcd.setCursor(3, 0);
    g_lcd.print("EAGLE RADIO");
    g_lcd.setCursor(3, 1);
    g_lcd.print(c_minFreq);
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

    delay(500);
    printTime(frequency);

    //// rotary encoder
    g_reClkState = digitalRead(c_reClk);
    g_reDatState = digitalRead(c_reDat);

    // mute swtich
    if (c_reSw == HIGH) {
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
}

void displayVolume() {
    string out_string;
    int count = g_volume * (16 / 18);

    out_string = string(count, "█") + string(16 - count, " ");

    if (g_muteState) {
        out_string = "MUTED";
    }

    g_lcd.clear();
    g_lcd.setCursor(3, 1);
    g_lcd.print(out_string);
}

void printTime(float frequency) {//default LCD output.
    /*
      |2020-13-45 15:00| >>> | 23 °
      |█████████       |
    OR
      |97.8HZ - EAGLE R| >>> | ADIO
    */
    string date = to_string(g_year) + '/' + to_string(g_month) + '/' + to_string(g_day);
    string time = to_string(g_hour) + ':' + to_string(g_minute);
    string temp = to_string(rtc.getTemp()) + 'C';
    string dateTimeTemp = date + time + temp;
    g_lcd.clear();
    g_lcd.setCursor(3, 0);
    g_lcd.print(dateTime);
    g_lcd.rightToLeft();
    g_lcd.setCursor(3, 1);
    g_lcd.print(frequency);//TODO: add station name;
}

string station_name(float frequency) {
}

void frequencyUpdate(float frequency) {
    // set the frequency
    g_radio.setFrequency(frequency);
    // print to the LCD
    g_lcd.clear();
    g_lcd.setCursor(3, 1); // TODO: test in labs
    g_lcd.print(frequency);
}
