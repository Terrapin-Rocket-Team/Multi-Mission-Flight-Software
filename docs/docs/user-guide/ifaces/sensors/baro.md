# Barometer Interface

The `Barometer` class in MMFS provides an abstract base for all barometric pressure sensors, extending the functionality of the core [`Sensor`](sensor.md) interface. It not only standardizes access to pressure data, but also computes altitude from that data, enabling derived classes to easily support telemetry and flight logic.

---

## **Overview**

Barometers convert pressure readings into meaningful altitude data using standard atmospheric models. The `Barometer` base class handles this transformation internally, so implementing a new sensor requires only providing raw pressure and temperature values via `read()`.

---

## **Inheritance Structure**

`Barometer` inherits from [`Sensor`](sensor.md) and implements many of the required pieces:

* `begin()` → Calls `init()` and handles sensor startup, including zeroing pressure to define "ground level"
* `update()` → Calls `read()` and populates altitude, pressure, and temperature values

The only two methods you must implement in a derived class is:

```cpp
void read() override;
bool init() override;
```
`read()` is intended to read the physical hardware data using the library you've found or written to talk to the sensor and update Barometer's internal variables.
`init()` is designed to use said library to initialize the sensor and write and config options that you need to set to it.

Neither of these methods are intended to any kind of complex math or variable transformation, with the potential exception of unit conversions. Anything more complex should already be handeld by Baromter's `update()` or `begin()`. Just double check that this abstract class doesn't already do the calculations that you're looking for before you write redundnat code.

---

## **Exposed Data Columns**

`Barometer` automatically registers the following telemetry columns:

* **Altitude** (`alt`) – computed from pressure
* **Pressure** (`press`) – hPa
* **Temperature** (`temp`) – °C

These values are exposed via the inherited [`DataReporter`](sensor.md#datareporter-integration) API and will appear in logs and live telemetry.

---

## **How Altitude is Calculated**

The barometer uses the hypsometric formula to convert pressure to altitude, assuming standard atmosphere:

```cpp
alt = 44330.0 * (1.0 - pow(pressure_hPa / basePressure_hPa, 0.1903));
```

Where `basePressure_hPa` is the pressure reading at ground level. This is automatically captured during `begin()`.

---

## **Usage Flow**

/// tab | Using a Barometer Sensor

Typical usage of a barometer sensor looks like:

```cpp
MyBMP390 bmp;
bmp.begin();

loop() {
    bmp.update();
    float alt = bmp.getAltitude();
    float temp = bmp.getTemperature();
}
```

You do **not** need to call `read()` manually—`update()` does that for you and stores the results internally.

///

/// tab | Implementing a New Hardware Sensor

To implement a new barometer driver, follow these steps:

1. **Choose a sensor library**

   * Prefer stable and maintained libraries (e.g., Adafruit, SparkFun)

2. **Wrap it in a new class derived from `Barometer`**

```cpp
class MySensor : public Barometer {
    AdafruitBMP390 hw;
public:
    bool begin(bool useBiasCorrection = true) override {
        hw.begin();
        return Barometer::begin(useBiasCorrection);
    }

    bool read(float &pressure_hPa, float &temperature_C) override {
        pressure_hPa = hw.readPressure();
        temperature_C = hw.readTemperature();
        return true;
    }
};
```

3. **Call `init()` and `update()` as needed**

   * `init()` should initialize your internal hardware and call `Barometer::begin()`
   * `update()` is already defined by `Barometer`, which will call your `read()` implementation

///

---

## **Available Built-in Drivers**

The following sensors are currently supported in MMFS:

* [BMP280](bmp280.md)
* [BMP390](bmp390.md)
* [DPS310](dps310.md)
* [MS5611](ms5611.md)

Each of these classes derives from `Barometer` and implements the required `read()` method.

---

## **Advanced Options**

### **Base Pressure Override**

If needed, you can override the zero-altitude baseline pressure after initialization:

```cpp
myBarometer.setBasePressure(1013.25);  // hPa
```

This is useful for resetting altitude calculations mid-flight or when dealing with unusual launch conditions.

### **Accessing Raw Data**

The following methods are available for reading the latest values:

```cpp
float getAltitude() const;
float getPressure() const;
float getTemperature() const;
```

---

## **Summary**

* `Barometer` simplifies pressure → altitude conversion using standard math
* Hardware implementations must only define a `read()` method
* `begin()` auto-calibrates base pressure and `update()` calls your driver
* Output is integrated with the MMFS logging/telemetry stack via `DataReporter`
* Easy to extend using any existing Arduino/C++ pressure sensor library

!!! note
    The `Barometer` class is meant for atmospheric sensors. Do **not** use it for water pressure sensors or sealed altimeters without modifying the pressure-to-altitude logic.

---

Written by ChatGPT. May not be fully accurate; verify against source.
