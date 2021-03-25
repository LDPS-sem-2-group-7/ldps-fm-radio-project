#ifndef constants_h
#define constants_h

const int c_i2cDataPath = 0x27;
const float c_minFreq = 96.4;
const float c_maxFreq = 107.9;
const float c_memFreq1 = 100.0; // TODO fix
const float c_memFreq2 = 101.0;
const int c_reDat = 3;
const int c_reClk = 2;
const int c_reSw = 4;
const int c_memButton1Pin = 5;
const int c_memButton2Pin = 6;
const int c_frequButton1Pin = 9;
const int c_frequButton2Pin = 10;
const int c_timeUp; //TODO some number;
const int c_timeDown; //TODO some number;
// TODO: Use pin buttons D7 and D8 to adjust RTC module
const int c_rtcPinSDA = 27; // this is just the i2c rail
const int c_rtcPinSCL = 28;
const int c_lcdLen = 16;
const int c_lcdHeight = 2;
const int c_minVolume = 0;
const int c_maxVolume = 18;


#endif
