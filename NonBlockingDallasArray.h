#ifndef NONBLOCKINGDALLASARRAY_H
#define NONBLOCKINGDALLASARRAY_H

#include "NonBlockingDallas.h"
#include <vector>


//#define DEBUG_NBDARRAY                //comment out if you want debug output 

#ifdef  DEBUG_NBDARRAY
#define _NBDARRAY_PP(a) Serial.print(a);
#define _NBDARRAY_PL(a) Serial.println(a);
#else
#define _NBDARRRY_PP(a)
#define _NBDARRAY_PL(a)
#endif


/*
The key purpose of the class is to group multiple NonBlockingDallas objects together
and provide methods to interact with all of them collectively. This allows you to manage
many temperature sensors over different wires efficiently.
*/
class NonBlockingDallasArray
{
private:
    NonBlockingDallas::NBD_unitsOfMeasure  _unitsOM;               //Unit of measurement
    NonBlockingDallas::NBD_resolution      _res;
    String              _pathofsensornames="";
    std::vector<NonBlockingDallas*> _wires;
public:
    NonBlockingDallasArray();
    ~NonBlockingDallasArray();

    void begin( NonBlockingDallas::NBD_resolution res, NonBlockingDallas::NBD_unitsOfMeasure uom, unsigned long tempInterval);
    void begin( NonBlockingDallas::NBD_resolution res, NonBlockingDallas::NBD_unitsOfMeasure uom, unsigned long tempInterval, String pathofsensornames);

    void addNonBlockingDallas(NonBlockingDallas* NBDpt);
    void                update();
    void                rescanWire();
    void                requestTemperature();
    const unsigned char getSensorsCount();
    void                saveSensorNames();
    String              addressToString(DeviceAddress devaddress);

    void                setResolution(NonBlockingDallas::NBD_resolution res);
    NonBlockingDallas::NBD_resolution      getResolution();

    void                setUnitsOfMeasure(NonBlockingDallas::NBD_unitsOfMeasure unit);
    NonBlockingDallas::NBD_unitsOfMeasure  getUnitsOfMeasure();
    String              getUnitsOfMeasureAsString();//'C' or 'F'

    unsigned char       getGPIO(unsigned char index,ENUM_NBD_ERROR &err);

    String              getWireName(unsigned char index);
    void                setWireName(String wirename, unsigned char index);

    float               getTempByIndex(unsigned char index, ENUM_NBD_ERROR &err);
    float               getTempByName(String name, ENUM_NBD_ERROR &err);
    float               getTempByNameS(String name);

    String              getSenorNameByIndex(unsigned char index, ENUM_NBD_ERROR &err);
    bool                setSenorNameByIndex(unsigned char index, String name, ENUM_NBD_ERROR &err);

    unsigned char       getIndexBySensorName(String name, ENUM_NBD_ERROR &err);
    ENUM_NBD_ERROR      getIndexBySensorName(String name, unsigned char &index);

    unsigned long       getLastTimeOfValidTempByIndex(unsigned char index, ENUM_NBD_ERROR &err);
    unsigned long       getLastTimeOfValidTempByName(const String& name, ENUM_NBD_ERROR &err);

    ENUM_NBD_ERROR      getAddressByIndex(unsigned char index, DeviceAddress &address);

    bool                setSensorNameByAddress(const DeviceAddress addr, String name, ENUM_NBD_ERROR &err);

};

#endif /* NONBLOCKINGDALLASARRAY_H */
