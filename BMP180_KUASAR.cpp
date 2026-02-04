#include "BMP180_KUASAR.h"

bool BMP180_KUASAR::begin(uint8_t sda, uint8_t scl, uint8_t oss) {
    _oss = oss;
    _error = false;
    Wire.begin(sda, scl);
    readCalibration();
    
    _fTemp = 0.0;
    _fPress = 0.0;
    _fAlt = 0.0;
    
    state = IDLE;
    startTemp();
    while(millis() - timer < 5){}
    update();
    while(!dataReady()){}
    return true;
}

void BMP180_KUASAR::update() {
    if (state == WAIT_TEMP && millis() - timer >= 5) {
        _temp = readRawTemp();
        startPressure();
    }
    else if (state == WAIT_PRESS) {
        int waitTime = (_oss==0)?5:(_oss==1)?8:(_oss==2)?14:26;
        if (millis() - timer >= waitTime) {
            _press = readRawPressure();

            float T = getTemperature();
            float P = getPressure();
            float alt = (pow(_seaLevel / P, 1.0/5.257) - 1.0) * (T+273.15) / 0.0065;

            _fTemp = iir_filter(T, _fTemp);
            _fPress = iir_filter(P, _fPress);
            _fAlt = iir_filter(alt, _fAlt);

            _newData = true;
            startTemp();
        }
    }
}

bool BMP180_KUASAR::dataReady() {
    if (_newData) {
        _newData = false;
        return true;
    }
    return false;
}

float BMP180_KUASAR::iir_filter(float in, float prev) {
    return prev + _alpha * (in - prev);
}

void BMP180_KUASAR::setIirFilter(float alpha) {
    _alpha = constrain(alpha, 0.01, 1.0);
}

void BMP180_KUASAR::calibrateSeaLevel(float knownAltitudeMeters) {
    float P = _fPress;
    _seaLevel = P / pow(1.0 - (0.0065 * knownAltitudeMeters) / (_fTemp + 273.15), 5.257);
}

float BMP180_KUASAR::getTemperature() {
    long X1 = ((_temp - AC6) * AC5) >> 15;
    long X2 = ((long)MC << 11)/(X1 + MD);
    long B5 = X1 + X2;
    return ((B5 + 8)>>4) / 10.0;
}

float BMP180_KUASAR::getPressure() {
    long rawT = _temp;
    long rawP = _press;

    long X1 = ((rawT - AC6) * AC5) >> 15;
    long X2 = ((long)MC << 11)/(X1 + MD);
    long B5 = X1 + X2;
    long B6 = B5 - 4000;

    X1 = (B2_cal * ((B6*B6)>>12)) >> 11;
    X2 = (AC2 * B6) >> 11;
    long X3 = X1 + X2;
    long B3 = (((((long)AC1)*4 + X3)<<_oss) + 2)>>2;

    X1 = (AC3 * B6) >> 13;
    X2 = (B1_cal * ((B6*B6)>>12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    unsigned long B4 = (AC4 * (unsigned long)(X3 + 32768)) >> 15;
    unsigned long B7 = ((unsigned long)rawP - B3)*(50000 >> _oss);

    long p;
    if(B7 < 0x80000000) p = (B7*2)/B4;
    else p = (B7/B4)*2;

    X1 = (p>>8)*(p>>8);
    X1 = (X1*3038)>>16;
    X2 = (-7357*p)>>16;
    p = p + ((X1 + X2 + 3791)>>4);

    return (float)p;
}

float BMP180_KUASAR::getAltitude() {
    return _fAlt;
}

void BMP180_KUASAR::startTemp() {
    Wire.beginTransmission(_BMP180_ADDR);
    Wire.write(0xF4); Wire.write(0x2E);
    Wire.endTransmission();
    timer = millis();
    state = WAIT_TEMP;
}

void BMP180_KUASAR::startPressure() {
    Wire.beginTransmission(_BMP180_ADDR);
    Wire.write(0xF4); Wire.write(0x34 + (_oss<<6));
    Wire.endTransmission();
    timer = millis();
    state = WAIT_PRESS;
}

int BMP180_KUASAR::readRawTemp() {
    Wire.beginTransmission(_BMP180_ADDR);
    Wire.write(0xF6);
    Wire.endTransmission();
    
    if (Wire.requestFrom(_BMP180_ADDR, 2) != 2) {
        _error = true;
        return 0;
    }
    return (Wire.read()<<8) | Wire.read();
}

long BMP180_KUASAR::readRawPressure() {
    Wire.beginTransmission(_BMP180_ADDR);
    Wire.write(0xF6);
    Wire.endTransmission();
    
    if (Wire.requestFrom(_BMP180_ADDR, 3) != 3) {
        _error = true;
        return 0;
    }
    long msb = Wire.read();
    long lsb = Wire.read();
    long xlsb = Wire.read();
    return ((msb<<16) | (lsb<<8) | xlsb) >> (8 - _oss);
}

void BMP180_KUASAR::readCalibration() {
    Wire.beginTransmission(_BMP180_ADDR);
    Wire.write(0xAA);
    Wire.endTransmission();
    
    if (Wire.requestFrom(_BMP180_ADDR, 22) != 22) {
        _error = true;
        return;
    }

    AC1 = (Wire.read()<<8) | Wire.read();
    AC2 = (Wire.read()<<8) | Wire.read();
    AC3 = (Wire.read()<<8) | Wire.read();
    AC4 = (Wire.read()<<8) | Wire.read();
    AC5 = (Wire.read()<<8) | Wire.read();
    AC6 = (Wire.read()<<8) | Wire.read();
    B1_cal = (Wire.read()<<8) | Wire.read();
    B2_cal = (Wire.read()<<8) | Wire.read();
    MB = (Wire.read()<<8) | Wire.read();
    MC = (Wire.read()<<8) | Wire.read();
    MD = (Wire.read()<<8) | Wire.read();
}

void BMP180_KUASAR::changeAddr(uint8_t address) {
    _BMP180_ADDR = address; 
}

bool BMP180_KUASAR::checkError() {
    return _error;
}