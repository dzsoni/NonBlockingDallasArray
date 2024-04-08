#ifndef NONBLOCKINGDALLASARRAY_H
#define NONBLOCKINGDALLASARRAY_H

#include "NonBlockingDallas.h"
#include <vector>

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
    String              getUnitsOfMeasureToString();//'C' or 'F'

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
    unsigned long       getLastTimeOfValidTempByName(String name, ENUM_NBD_ERROR &err);

    ENUM_NBD_ERROR      getAddressByIndex(unsigned char index, DeviceAddress &address);

    bool                setSensorNameByAddress(const DeviceAddress addr, String name, ENUM_NBD_ERROR &err);

};

#endif /* NONBLOCKINGDALLASARRAY_H */
