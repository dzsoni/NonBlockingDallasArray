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

#include "NonBlockingDallas.h"
#include <algorithm>
#include <wstring.h>

NonBlockingDallas::NonBlockingDallas(DallasTemperature *dallasTemp, unsigned char pin)
{
    _gpiopin = pin;
    _dallasTemp = dallasTemp;
    _lastReadingMillis = 0;
    _startConversionMillis = 0;
    _conversionMillis = 0;
    _currentState = notFound;
    cb_onIntervalElapsed = NULL;
    cb_onTemperatureChange = NULL;
    _pathofsensornames = "";
    _wireName = String("GPIO"); // Set default wire name
    _wireName += String(_gpiopin);
}

NonBlockingDallas::NonBlockingDallas(DallasTemperature *dallasTemp, unsigned char pin, String pathofsensornames)
{
    _gpiopin = pin;
    _dallasTemp = dallasTemp;
    _lastReadingMillis = 0;
    _startConversionMillis = 0;
    _conversionMillis = 0;
    _currentState = notFound;
    cb_onIntervalElapsed = NULL;
    cb_onTemperatureChange = NULL;
    _wireName =  String("GPIO"); // Set default wire name
    _wireName += String(_gpiopin);
    _pathofsensornames = pathofsensornames;
}

void NonBlockingDallas::begin(NBD_resolution res, NBD_unitsOfMeasure uom, unsigned long tempInterval)
{
    _res = res;
    _tempInterval = tempInterval;
    _unitsOM = uom;
    _currentState = notFound;
    _conversionMillis = DallasTemperature::millisToWaitForConversion(_res);
    rescanWire();

    if ((_tempInterval < _conversionMillis) || (_tempInterval > 4294967295UL))
    {
        _tempInterval = DEFAULT_INTERVAL;

        _DS18B20_PP(F("DS18B20: temperature interval not valid. Setting the default value: "));
        _DS18B20_PL(DEFAULT_INTERVAL);
    }

    _DS18B20_PP(F("DS18B20: "));
    _DS18B20_PP(getSensorsCount());
    _DS18B20_PL(F(" sensors found on the bus"));
#ifdef DEBUG_DS18B20
    if (getSensorsCount() > 0)
    {
        _DS18B20_PP(F("DS18B20: parasite power is "));
        if (_dallasTemp->isParasitePowerMode())
        {
            _DS18B20_PP(F("ON"));
        }
        else
        {
            _DS18B20_PP(F("OFF"));
        }
    }
    _DS18B20_PL("");
#endif
}

//==============================================================================================
//                                  PRIVATE
//==============================================================================================



void NonBlockingDallas::waitNextReading()
{
    //if (_lastReadingMillis != 0 && (millis() - _lastReadingMillis < _tempInterval - _conversionMillis))
    if (_lastReadingMillis != 0 && (millis() - _lastReadingMillis < _tempInterval))
        return;
    requestTemperature();
}

void NonBlockingDallas::waitConversionAndRead()
{
    if (!_dallasTemp->isConversionComplete())
        return;

    // Save the actual sensor conversion time to precisely calculate the next reading time
    _conversionMillis = millis() - _startConversionMillis;
    Serial.println("Conversion takes:" + String(_conversionMillis) + " ms");
    readSensors();
}

void NonBlockingDallas::readSensors()
{
    for (int i = 0; i < getSensorsCount(); i++)
    {
        readTemperatures(i);
#ifdef DEBUG_DS18B20
        ENUM_NBD_ERROR err;
        _DS18B20_PP(F("Sensor ("));
        _DS18B20_PP(i);
        _DS18B20_PP("): ");
        _DS18B20_PP(getTempByIndex(i, err));

        if (_unitsOM == unit_C)
        {
            _DS18B20_PP(" °C ");
        }
        else
        {
            _DS18B20_PP(" °F ");
        }
        _DS18B20_PP("Family code:");
#define ADR(a, i) _sdv.at(i).sensorAddress[a]
        _DS18B20_PP(ADR(0, i));
        _DS18B20_PP(" ");
        _DS18B20_PP("Serial:")
        _DS18B20_PP(ADR(1, i));
        _DS18B20_PP("-");
        _DS18B20_PP(ADR(2, i));
        _DS18B20_PP("-");
        _DS18B20_PP(ADR(3, i));
        _DS18B20_PP("-");
        _DS18B20_PP(ADR(4, i));
        _DS18B20_PP("-");
        _DS18B20_PP(ADR(5, i));
        _DS18B20_PP("-");
        _DS18B20_PP(ADR(6, i));
        _DS18B20_PP(" CRC:");
        _DS18B20_PP(ADR(7, i));
        _DS18B20_PL("");

#endif
    }

    _lastReadingMillis = millis();
    _currentState = waitingNextReading;
}

void NonBlockingDallas::readTemperatures(int deviceIndex)
{
    float temp = (_unitsOM==unit_C) ? DEVICE_DISCONNECTED_C : DEVICE_DISCONNECTED_F;
    bool validReadout = false;
    switch (_unitsOM)
    {
    case unit_C:
        temp = _dallasTemp->getTempC(_sdv.at(deviceIndex).sensorAddress);
        validReadout = (temp != DEVICE_DISCONNECTED_C); //-127.0
        break;
    case unit_F:
        temp = _dallasTemp->getTempF(_sdv.at(deviceIndex).sensorAddress);
        validReadout = (temp != DEVICE_DISCONNECTED_F); //-196.6
        break;
    }
    if (_sdv.at(deviceIndex).temperature != temp && validReadout)
    {
        if (cb_onTemperatureChange)
            (*cb_onTemperatureChange)(temp, validReadout, _wireName, getGPIO(), deviceIndex);
    }

    if (validReadout)
    {
        _sdv.at(deviceIndex).lastTimeOfValidTemp = millis();
    }
    _sdv.at(deviceIndex).temperature = temp;
    _sdv.at(deviceIndex).valid=validReadout;

    if (cb_onIntervalElapsed)
        (*cb_onIntervalElapsed)(temp, validReadout, _wireName, getGPIO(), deviceIndex);
}

//==============================================================================================
//                                  PUBLIC
//==============================================================================================

void NonBlockingDallas::update()
{
    switch (_currentState)
    {
    case notFound:

        break;
    case waitingNextReading:
        waitNextReading();
        break;
    case waitingConversionAndRead:
        waitConversionAndRead();
        break;
    }
}

void NonBlockingDallas::requestTemperature()
{
    _currentState = waitingConversionAndRead;
    _startConversionMillis = millis();
    _dallasTemp->requestTemperatures(); // Requests a temperature conversion for all the sensors on the bus

    _DS18B20_PL(F("DS18B20: requested new reading."));
}

void NonBlockingDallas::rescanWire()
{
    _dallasTemp->begin();
    _dallasTemp->setWaitForConversion(false); // Avoid blocking the CPU waiting for the sensors conversion
    _currentState = notFound;
    
    DeviceAddress newaddress;
    _sdv.clear();

    _currentState = waitingNextReading;
    _dallasTemp->setResolution((uint8_t)_res);

    _DS18B20_PL(String(__FUNCTION__)+" sensors count:"+String(getSensorsCount()));
    
    for (int i = 0; i < getSensorsCount(); i++)
    {  
        _sdv.emplace_back();
        if (_dallasTemp->getAddress(&newaddress[0], i))
        {
            for (size_t a = 0; a < 8; a++)
            {
                _sdv.at(i).sensorAddress[a] = newaddress[a];
            }
            if(_pathofsensornames!= "")
            {
            ENUM_NBD_ERROR err = NBD_NO_ERROR;
            setSensorNameByAddress(_sdv.at(i).sensorAddress,
                                   _sjsonp.getValueByKeyFromFile(_pathofsensornames,
                                                                 addressToString(_sdv.at(i).sensorAddress)),
                                   err);
            if(err!=NBD_NO_ERROR)
            {
             _DS18B20_PL(String(__FUNCTION__)+F(" Error setting sensor name by address. NBD error code:")+String(err));
            }
            }
        }
    }
    _sdv.shrink_to_fit();
}

ENUM_NBD_ERROR NonBlockingDallas::getAddressByIndex(unsigned char index, DeviceAddress &address)
{
    if (index >= getSensorsCount())
    {
        return NBD_INDEX_IS_OUT_OF_RANGE;
    }
    for (size_t i = 0; i < 8; i++)
    {
        address[i] = _sdv.at(index).sensorAddress[i];
    }
    return NBD_NO_ERROR;
}

const unsigned char NonBlockingDallas::getSensorsCount()
{
    return _dallasTemp->getDeviceCount();
}

float NonBlockingDallas::getTempByIndex(unsigned char index, ENUM_NBD_ERROR &err)
{
    if (index >= getSensorsCount())
    {
        err = NBD_INDEX_IS_OUT_OF_RANGE;
        return (_unitsOM==unit_C) ? DEVICE_DISCONNECTED_C : DEVICE_DISCONNECTED_F;
    }
    err=NBD_NO_ERROR;
    return _sdv.at(index).temperature;
}

unsigned char NonBlockingDallas::getGPIO()
{
    return _gpiopin;
}


float NonBlockingDallas::getTempByName(String name, ENUM_NBD_ERROR &err)
{
    auto compareNames = [&](const SensorData &sd)
    { return sd.sensorName == name; };
    auto it = std::find_if(_sdv.begin(), _sdv.end(), compareNames);
    if (it == _sdv.end())
    {
        err = NBD_NAME_NOT_FOUND;
        return (_unitsOM==unit_C) ? DEVICE_DISCONNECTED_C : DEVICE_DISCONNECTED_F;
    }
    err=NBD_NO_ERROR;
    return it->temperature;
}

unsigned long NonBlockingDallas::getLastTimeOfValidTempByName(const String name, ENUM_NBD_ERROR &err)
{
    auto compareNames = [&](const SensorData &sd)
    { return sd.sensorName == name; };
    auto it = std::find_if(_sdv.begin(), _sdv.end(), compareNames);
    if (it == _sdv.end())
    {
        err = NBD_NAME_NOT_FOUND;
        return (_unitsOM==unit_C) ? DEVICE_DISCONNECTED_C : DEVICE_DISCONNECTED_F;
    }
    err=NBD_NO_ERROR;
    return it->lastTimeOfValidTemp;
}

unsigned long NonBlockingDallas::getLastTimeOfValidTempByIndex(unsigned char index, ENUM_NBD_ERROR &err)
{
    if (index >= getSensorsCount())
    {
        err = NBD_INDEX_IS_OUT_OF_RANGE;
        return 0UL;
    }
    err=NBD_NO_ERROR;
    return _sdv.at(index).lastTimeOfValidTemp;
}

bool NonBlockingDallas::setSensorNameByIndex(unsigned char index, String name, ENUM_NBD_ERROR &err)
{
    if (index >= getSensorsCount())
    {
        err = NBD_INDEX_IS_OUT_OF_RANGE;
        return false;
    }
     err=NBD_NO_ERROR;
    _sdv.at(index).sensorName = name;
    return true;
}

unsigned char NonBlockingDallas::getIndexBySensorName(String name, ENUM_NBD_ERROR &err)
{
    auto compareNames = [&](const SensorData &sd)
    { return sd.sensorName == name; };
    auto it = std::find_if(_sdv.begin(), _sdv.end(), compareNames);
    if (it == _sdv.end())
    {
        err = NBD_NAME_NOT_FOUND;
        return 0;
    }
    err=NBD_NO_ERROR;
    return (unsigned char)std::distance(_sdv.begin(), it);
}

ENUM_NBD_ERROR NonBlockingDallas::getIndexBySensorName(String name, unsigned char &index)
{
    auto compareNames = [&](const SensorData &sd)
    { return sd.sensorName == name; };
    auto it = std::find_if(_sdv.begin(), _sdv.end(), compareNames);
    if (it == _sdv.end())
    {
        return NBD_NAME_NOT_FOUND;
    }
    index = std::distance(_sdv.begin(), it);
    return NBD_NO_ERROR;
}

String NonBlockingDallas::getSensorNameByIndex(unsigned char index, ENUM_NBD_ERROR &err)
{
    if (index >= getSensorsCount())
    {
        err = NBD_INDEX_IS_OUT_OF_RANGE;
        return String("");
    }
    err=NBD_NO_ERROR;
    return _sdv.at(index).sensorName;
}

bool NonBlockingDallas::setSensorNameByAddress(const DeviceAddress addr, String name, ENUM_NBD_ERROR &err)
{
    bool found;
    int i = 0;
    for ( i ; i < getSensorsCount(); i++)
    {
        found=true;
        for (auto t = 0; t < 8; t++)
        {
            if (_sdv.at(i).sensorAddress[t] != addr[t])
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            _sdv.at(i).sensorName=name;
            err=NBD_NO_ERROR;
            return true;
        }
    }
    err = NBD_ADDRESS_IS_NOT_FOUND;
    return false;
}


void NonBlockingDallas::setPathOfSensorNames(String path)
{
    _pathofsensornames = path;
}

void NonBlockingDallas::setUnitsOfMeasure(NBD_unitsOfMeasure unit)
{
    _unitsOM=unit;
}

NonBlockingDallas::NBD_unitsOfMeasure NonBlockingDallas::getUnitsOfMeasure()
{
    return _unitsOM;
}

String NonBlockingDallas::getUnitsOfMeasureAsString()
{
    return (_unitsOM==unit_C) ? "C" :"F";
}

String NonBlockingDallas::addressToString(DeviceAddress sensorAddress)
{
    String str = "";
    for (int i = 0; i < 8; i++)
    {
        str += sensorAddress[i];
        if (i != 7)
            str += ".";
    }
    return str;
}

/**
 * Returns the wire name for the NonBlockingDallas instance.
 *
 * @return the wire name
 */
String NonBlockingDallas::getWireName()
{
    return _wireName;
}

/**
 * Sets the wire name for the NonBlockingDallas object.
 *
 * @param wirename the new wire name to set
 */
void NonBlockingDallas::setWireName(String wirename)
{
    _wireName = wirename;
}

 
/**
 * Saves the sensor names to a file. You should set the path of the file
 * using setPathOfSensorNames before calling this method, or construct the NonBlockingDallas object with a path.
 * Note: The file will be created if it doesn't exist, and won't be created if the path doesn't exist.
 * @param None
 *
 * @return None
 */
void NonBlockingDallas::saveSensorNames()
{
    if (_pathofsensornames != "")
    {
        String json = "{";
        for (unsigned int i = 0; i < _sdv.size(); i++)
        {
            if (i != 0)
                json += ",";
            json += "\"";
            json += addressToString(_sdv[i].sensorAddress);
            json += "\":\"";
            json += _sdv[i].sensorName;
            json += "\"";
        }
        json += "}";

        File file = SPIFFS.open(_pathofsensornames, "w");
        if (!file)
        {
            _DS18B20_PL(F("Error opening file for writing"));
            return;
        }
        _DS18B20_PL(json);
        file.print(json);
        file.flush();
        file.close();
    }
}

/**
 * Set the resolution for NonBlockingDallas sensor.
 *
 * @param res the resolution to be set
 */
void NonBlockingDallas::setResolution(NBD_resolution res)
{
    _res=res;
}

/**
 * Get the resolution value from the NonBlockingDallas class.
 *
 * @return the resolution value
 */
NonBlockingDallas::NBD_resolution NonBlockingDallas::getResolution()
{
    return _res;
}
