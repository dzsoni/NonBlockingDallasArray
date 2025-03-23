# Non blocking temperature sensor library for Arduino

The NonBlockingDallasArray library is designed to manage multiple NonBlockingDallas objects ( different from the original Gbertaz's NonblockigDallas lib), which themselves handle DS18B20 temperature sensors in a non-blocking manner. This library provides a higher-level abstraction for managing multiple OneWire buses, each with its own set of sensors, allowing for centralized control and data aggregation.

## Key Features of the NonBlockingDallasArray Library:
- Centralized Management:
  - Groups multiple NonBlockingDallas objects into a single collection.
  - Provides methods to interact with all NonBlockingDallas objects collectively.
- Non-Blocking Design:
  - Maintains the non-blocking nature of the NonBlockingDallas class, ensuring efficient operation without halting the main program loop.
- Sensor Operations:
  - Supports operations like updating sensors, rescanning wires, and requesting temperatures for all sensors across all wires.
- Data Aggregation:
  - Aggregates data such as sensor counts, temperatures, and sensor names from all managed NonBlockingDallas objects.
- Utility Functions:
  - Includes helper functions for tasks like converting sensor addresses to strings, setting sensor names, and retrieving sensor data by index or name.
- Customizable Settings:
  - Allows setting resolution, units of measurement (Celsius or Fahrenheit), and sensor reading intervals for all managed NonBlockingDallas objects.
- File-Based Sensor Name Management:
  - Supports saving and loading sensor names to/from a file in JSON format, enabling persistent sensor identification.
- Error Handling:
  - Provides detailed error codes (ENUM_NBD_ERROR) for operations, such as when a sensor index is out of range or a sensor name is not found.

## Initialisation

```
#include <OneWire.h>
#include <NonBlockingDallas.h>
#include <NonBlockingDallasArray.h>

#define ONE_WIRE_BUS1 12  //Sensors on GPIO 12
#define ONE_WIRE_BUS2 14  //Sensors on GPIO 14
#define SENSOR_NAMES_JSON "/sensnames.json"

OneWire oneWire1(ONE_WIRE_BUS1);
OneWire oneWire2(ONE_WIRE_BUS2);

DallasTemperature dallasTemp1(&oneWire1);
DallasTemperature dallasTemp2(&oneWire2);

NonBlockingDallas nonblocking_1(&dallasTemp1, ONE_WIRE_BUS1, SENSOR_NAMES_JSON);
NonBlockingDallas nonblocking_2(&dallasTemp2, ONE_WIRE_BUS2, SENSOR_NAMES_JSON);

nonblocking_1.onIntervalElapsed(handleIntervalElapsed);
nonblocking_1.onTemperatureChange(handleTemperatureChange);
nonblocking_2.onIntervalElapsed(handleIntervalElapsed);
nonblocking_2.onTemperatureChange(handleTemperatureChange);

NonBlockingDallasArray NBDArray;
NBDArray.addNonBlockingDallas(&nonblocking_1);
NBDArray.addNonBlockingDallas(&nonblocking_2);

NBDArray.begin(NonBlockingDallas::NBD_resolution::resolution_12,NonBlockingDallas::NBD_unitsOfMeasure::unit_C,DEFAULT_INTERVAL,SENSOR_NAMES_JSON);

```
While the sensnames.json looks like this:
```
{"40.187.127.121.162.0.3.131":"tempA",
 "40.140.59.118.224.1.60.194":"tempB",
 "40.123.5.118.224.1.60.19":"tempC"}
```
