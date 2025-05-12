# Sensor Interface

The `Sensor` interface in MMFS defines a standard structure for hardware sensor modules such as barometers, GPS units, IMUs, and more. It provides a unified API for initialization, updates, and data reporting. All sensors inherit from the `Sensor` base class, which itself inherits from `DataReporter`, enabling each sensor to automatically expose its data in a standardized way for logging or telemetry.

---

## **Purpose**

This interface ensures that all sensor types in MMFS can be:

* Queried consistently (via `update()`)
* Initialized uniformly (via `begin()`)
* Identified programmatically by type
* Hooked into telemetry/logging systems via inherited `DataReporter`

---

## **Core Methods**

### **Initialization**

```cpp
virtual bool begin(bool useBiasCorrection = true) = 0;
```

Prepares the sensor for use. May involve hardware initialization, I2C/SPI setup, and calibration. Some sensors support optional bias correction at startup.

### **Update**

```cpp
virtual void update() = 0;
```

Fetches new data from the sensor. This function is called periodically (e.g., once per loop) and internally handles reading from the sensor and updating internal data buffers.

### **Type Identification**

```cpp
virtual const SensorType getType() const = 0;
virtual const char *getTypeString() const = 0;
```

Returns the enum or string identifier for the sensor type. Useful for dynamic system configuration, debug messages, or logging.

---

## **DataReporter Integration**

Because `Sensor` inherits from `DataReporter`, it can expose internal sensor values to the logging and telemetry system.

To expose a new value:

```cpp
addColumn(FLOAT, &temperature, "Temp");
```

To remove a value:

```cpp
removeColumn("Temp");
```

The `DataReporter` base handles linked list management of these columns, automatic formatting of values, and integration with the Logger.

---

## **Bias Correction Controls**

### **Bias Mode Toggle**

```cpp
void setBiasCorrectionMode(bool mode);
```

Determines whether the sensor should continuously zero itself using incoming data (Used before liftoff to prevent long-term drift).

### **Liftoff Lock-in**

```cpp
void markLiftoff();
```

Disables any ongoing bias correction. Typically called once the rocket leaves the pad.

---

## **Implementing Your Own Sensor**

To implement your own sensor, inherit from `Sensor` and define the following:

```cpp
class MySensor : public Sensor {
public:
    bool begin(bool useBiasCorrection = true) override;
    void update() override;
    SensorType getType() const override { return OTHER_; }
    const char* getTypeString() const override { return "MY_SENSOR"; }
};
```

In the constructor, register your output values using the inherited `addColumn()` methods.

### **Example Constructor**

```cpp
MySensor::MySensor() {
    addColumn(FLOAT, &someValue, "Some Value");
    addColumn(BOOL, &statusFlag, "Status");
}
```

---

## **SensorType Enum**

```cpp
enum SensorType {
    BAROMETER_,
    GPS_,
    IMU_,
    LIGHT_SENSOR_,
    RADIO_,
    RTC_,
    ENCODER_,
    OTHER_
};
```

Use these types to categorize your sensor. If none fit, use `OTHER_`.

---

## **Summary**

* All sensors inherit from `Sensor`, which requires `begin()`, `update()`, and type reporting methods.
* Sensors automatically plug into the telemetry/logging system via `DataReporter`.
* Bias correction and liftoff control allow for zeroing and stabilization of certain sensors.
* Extending the system with new sensors is easy—just inherit and override.

---

Written by ChatGPT. May not be fully accurate; verify before flight.
