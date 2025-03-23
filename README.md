# Non blocking temperature sensor library for Arduino

The NonBlockingDallasArray library is designed to manage multiple NonBlockingDallas objects (modified original , which themselves handle DS18B20 temperature sensors in a non-blocking manner. This library provides a higher-level abstraction for managing multiple OneWire buses, each with its own set of sensors, allowing for centralized control and data aggregation.

Key Features of the NonBlockingDallasArray Library:
Centralized Management:

Groups multiple NonBlockingDallas objects into a single collection.
Provides methods to interact with all NonBlockingDallas objects collectively.
Non-Blocking Design:

Maintains the non-blocking nature of the NonBlockingDallas class, ensuring efficient operation without halting the main program loop.
Sensor Operations:

Supports operations like updating sensors, rescanning wires, and requesting temperatures for all sensors across all wires.
Data Aggregation:

Aggregates data such as sensor counts, temperatures, and sensor names from all managed NonBlockingDallas objects.
Utility Functions:

Includes helper functions for tasks like converting sensor addresses to strings, setting sensor names, and retrieving sensor data by index or name.
Customizable Settings:

Allows setting resolution, units of measurement (Celsius or Fahrenheit), and sensor reading intervals for all managed NonBlockingDallas objects.
File-Based Sensor Name Management:

Supports saving and loading sensor names to/from a file in JSON format, enabling persistent sensor identification.
Error Handling:

Provides detailed error codes (ENUM_NBD_ERROR) for operations, such as when a sensor index is out of range or a sensor name is not found.
