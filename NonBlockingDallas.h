// MIT License
//
// Copyright(c) 2021 Giovanni Bertazzoni <nottheworstdev@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef NONBLOCKINGDALLAS_H
#define NONBLOCKINGDALLAS_H


#include <Arduino.h>
#include <DallasTemperature.h>
#include <SimpleJsonParser.h> //https://github.com/dzsoni/SimpleJsonParser
#include "NBD_errorcodes.h"
#include <vector>

//#define DEBUG_DS18B20

#ifdef DEBUG_DS18B20
#define _DS18B20_PP(a) Serial.print(a);
#define _DS18B20_PL(a) Serial.println(a);
#else
#define _DS18B20_PP(a)
#define _DS18B20_PL(a)
#endif

#define DEFAULT_INTERVAL 31000
#define ONE_WIRE_MAX_DEV 15 //Maximum number of devices on the One wire bus

struct SensorData
{
    float temperature = DEVICE_DISCONNECTED_C;              //Last temperature value
    DeviceAddress sensorAddress = {0, 0, 0, 0, 0, 0, 0, 0}; //Array of sensors' address
    unsigned long lastTimeOfValidTemp = 0;                 //Last valid reading time of a temp
    bool valid = false;
    String sensorName = "";                                 //Name of the sensor
};


class NonBlockingDallas
{
public:
   enum NBD_resolution
    {
        resolution_9 = 9,
        resolution_10 = 10,
        resolution_11 = 11,
        resolution_12 = 12
    };
   enum NBD_unitsOfMeasure
    {
        unit_C,
        unit_F
    };
    NonBlockingDallas(DallasTemperature *dallasTemp, unsigned char pin);
    NonBlockingDallas(DallasTemperature *dallasTemp, unsigned char pin, String pathofsensornames);

    void                begin(NBD_resolution res, NBD_unitsOfMeasure uom, unsigned long tempInterval);
    void                update();
    void                rescanWire();
    void                requestTemperature();
    const unsigned char getSensorsCount();
    unsigned char       getGPIO();
    void                setUnitsOfMeasure(NBD_unitsOfMeasure unit);
    NBD_unitsOfMeasure  getUnitsOfMeasure();
    String              getUnitsOfMeasureAsString();//'C' or 'F'
    String              addressToString(DeviceAddress sensorAddress);

    String              getWireName();
    void                setWireName(String wirename);

    void                saveSensorNames();

    void                setResolution(NBD_resolution res);
    NBD_resolution      getResolution();

    void                setPathOfSensorNames(String path);
    
    float               getTempByIndex(unsigned char index, ENUM_NBD_ERROR &err);
    float               getTempByName(String name, ENUM_NBD_ERROR &err);

    String              getSensorNameByIndex(unsigned char index, ENUM_NBD_ERROR &err);

    unsigned char       getIndexBySensorName(String name, ENUM_NBD_ERROR &err);
    ENUM_NBD_ERROR      getIndexBySensorName(String name, unsigned char &index);

    ENUM_NBD_ERROR      getAddressByIndex(unsigned char index, DeviceAddress& address);

    unsigned long       getLastTimeOfValidTempByIndex(unsigned char index, ENUM_NBD_ERROR &err);
    unsigned long       getLastTimeOfValidTempByName(const String name, ENUM_NBD_ERROR &err);

    bool                setSensorNameByIndex(unsigned char index, String name, ENUM_NBD_ERROR &err);
    bool                setSensorNameByAddress(const DeviceAddress addr, String name, ENUM_NBD_ERROR &err);

    void onIntervalElapsed(void (*callback)(float temperature, bool valid, String wname, unsigned char gpiopin, int deviceIndex))
    {
        cb_onIntervalElapsed = callback;
    }
    void onTemperatureChange(void (*callback)(float temperature, bool valid, String wname, unsigned char gpiopin, int deviceIndex))
    {
        cb_onTemperatureChange = callback;
    }

private:
    enum sensorState
    {
        notFound = 0,
        waitingNextReading,
        waitingConversionAndRead,
    };
    String              _wireName; //Name of the wire
    SimpleJsonParser    _sjsonp;
    unsigned char       _gpiopin;
    NBD_resolution      _res;
    DallasTemperature   *_dallasTemp;
    sensorState         _currentState;
    unsigned long       _lastReadingMillis;     //Time at last temperature sensor readout
    unsigned long       _startConversionMillis; //Time at start conversion of the sensor
    unsigned long       _conversionMillis;      //Sensor conversion time based on the resolution [milliseconds]
    unsigned long       _tempInterval;          //Interval among each sensor reading [milliseconds]
    NBD_unitsOfMeasure  _unitsOM;               //Unit of measurement
    String _pathofsensornames;

    std::vector<SensorData> _sdv = std::vector<SensorData>(); //every sensors' data on this wire

    void waitNextReading();
    void waitConversionAndRead();
    void readSensors();
    void readTemperatures(int deviceIndex);
    void (*cb_onIntervalElapsed)(float temperature, bool valid, String wname, unsigned char gpiopin,  int deviceIndex);
    void (*cb_onTemperatureChange)(float temperature, bool valid, String wname, unsigned char gpiopin,  int deviceIndex);
};
#endif /* NONBLOCKINGDALLAS_H */
