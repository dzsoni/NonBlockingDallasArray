#ifndef NBD_ERRORCODES_H
#define NBD_ERRORCODES_H


enum ENUM_NBD_ERROR
{
    NBD_NO_ERRROR =0,              //No error.
    NBD_INDEX_IS_OUT_OF_RANGE,     //Index is out of bound.
    NBD_ADDRESS_IS_NOT_FOUND,      //Device address is not found.
    NBD_NAME_NOT_FOUND             //Name is not found.
};
#endif