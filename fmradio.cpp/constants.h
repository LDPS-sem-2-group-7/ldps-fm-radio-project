#ifndef constants_h
#define constants_h

const int c_i2cDataPath = 0x27;
const float c_minFreq = 88.0;
const float c_maxFreq = 108.0;
const float c_minFreqKnown = 88.1;
const float c_maxFreqKnown = 105.4;
const float c_memFreq1 = 96.4; // Eagle radio
const float c_memFreq2 = 104.7; // BBC Surrey
const int c_reDat = 3;
const int c_reClk = 2;
const int c_reSw = 4;

const int c_memButton1Pin = 5;
const int c_memButton2Pin = 6;
const int c_frequButton2Pin = 7;
const int c_frequButton1Pin = 8;
const int c_timeMin = 9;
const int c_timeHour = 10;


const int c_rtcPinSDA = 27; // this is just the i2c rail
const int c_rtcPinSCL = 28;
const int c_lcdLen = 16;
const int c_lcdHeight = 2;
const int c_minVolume = 0;
const int c_maxVolume = 18;
const int c_defaultVolume = 10;
const int c_tickDelay = 40;


#endif
