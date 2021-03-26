# ldps-fm-radio-project
This repository holds all the relevant libraries and files needed to make a FM Radio using an Arduino nano, an Audio Amplifer and a Radio chip.

For our project we are using the AR1010 radio chip, a 16x2 LCD display and a DS3231 RTC module.

## Features
The FM radio can do all of the following:
  * Play music from radio station in the Fm range.
  * Adjust volume.
  * Seek frequencies, both up and down.
  * Display time and temperature.
  * Adjust time
  * Pre-set memory buttons to store 2 stations.
  * Headphone jack out


## Structure
The skeleton.cpp file holds all of the code that is run by the Arduino. It contains a setup() and loop() function.

The setup() function is what happens when the arduino is first turned on.
From then on, whatever is within the loop() function will be executed recursively.

displayVolume(), frequencyUpdate() and printTime() are functions we made to show the volume changing and time repectively.
These are called within the loop function as the FM Radio defaults to showing the time and current frequency when the system is idle.
Each time the frequency is being changed, we use the frequencyUpdate() function to show the frequency changing.
When the volume rotary encoder is turned then we show the volume increasing and decreasing on the screen.

## Prerequisites
Please find the following prerequisites inside the repo.
  * AR1010lib.h & A1010lib.cpp
  * LiquidCrystal_I2C.h & LiquidCrystal_I2C.cpp
  * DS3231.h & DS3231.cpp





