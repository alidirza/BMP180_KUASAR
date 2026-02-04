#ifndef BMP180_KUASAR_H
#define BMP180_KUASAR_H

#include <Arduino.h>
#include <Wire.h>


class BMP180_KUASAR {
public:
    bool begin(uint8_t sda, uint8_t scl, uint8_t oss = 3);

    void update();                      
    bool dataReady();                   

    float getTemperature();             
    float getPressure();                
    float getAltitude();                

    void calibrateSeaLevel(float knownAltitudeMeters); 
    void setIirFilter(float alpha);        // 0.0–1.0 arası (küçük = daha yumuşak), 0.2 roket için ideal gibi
    void changeAddr(uint8_t address);
    bool checkError();                     

private:
    enum State { IDLE, WAIT_TEMP, WAIT_PRESS } state;

    void readCalibration();
    void startTemp();
    void startPressure();

    int readRawTemp();
    long readRawPressure();


    float iir_filter(float input, float prev);

    uint8_t _oss;
    unsigned long timer;

    int _temp;
    long _press;
    float _alt;
    float _fTemp, _fPress, _fAlt;
    float _alpha = 0.2;

    float _seaLevel = 101325.0;
    uint8_t _BMP180_ADDR = 0x77;
    bool _newData = false;
    bool _error = false;

    int16_t AC1, AC2, AC3, B1_cal, B2_cal, MB, MC, MD;
    uint16_t AC4, AC5, AC6;
};

#endif
