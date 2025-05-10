# Barometer

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

3. If you're using **MMFSSystem**, then that's it. Pass an instance of your new barometer to State, and the software will take care of the rest. If you're not using MMFSSystem, you should call `myBaro.begin()` in `setup()` and `myBaro.update()` in `loop()`, at whatever frequency you find works best.

---

## **Available Built-in Drivers**

The following sensors are currently supported in MMFS:

* [BMP280](https://github.com/Terrapin-Rocket-Team/Multi-Mission-Flight-Software/blob/main/src/Sensors/Baro/BMP280.h) - [Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf) (**NRND**)
* [BMP390](https://github.com/Terrapin-Rocket-Team/Multi-Mission-Flight-Software/blob/main/src/Sensors/Baro/BMP390.h) - [Datasheet](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp390-ds002.pdf)
* [DPS310/DPS368](https://github.com/Terrapin-Rocket-Team/Multi-Mission-Flight-Software/blob/main/src/Sensors/Baro/DPS310.h) - [Datasheet](https://www.infineon.com/dgdl/Infineon-DPS310-DataSheet-v01_02-EN.pdf?fileId=5546d462576f34750157750826c42242) (DPS310 is **NRND**) and [Datasheet](https://www.infineon.com/dgdl/Infineon-DPS368-DataSheet-v01_01-EN.pdf?fileId=5546d46269e1c019016a0c45105d4b40)
* [MS5611](https://github.com/Terrapin-Rocket-Team/Multi-Mission-Flight-Software/blob/main/src/Sensors/Baro/MS5611F.h) - [Datasheet](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5611-01BA03%7FB3%7Fpdf%7FEnglish%7FENG_DS_MS5611-01BA03_B3.pdf%7FCAT-BLPS0036)

Each of these classes derives from `Barometer` and implements the required `read()` and `init()` methods.

---

## **Advanced Options**

### **Using Bias Correction Mode**

Bias correction mode helps compensate for slow pressure drift and sensor noise by continuously recalibrating the zero-altitude baseline — essentially adjusting what the barometer considers "ground level." This is especially useful when the vehicle stays on the pad for a long time before launch, or when ambient pressure changes slightly.

However, it’s off by default because blindly recalibrating can be dangerous — especially if the rocket has already left the ground. See the MMFSSystem docs for more on the risks and defaults.

When enabled, the barometer periodically computes a new baseline using the second-to-last second of recent pressure data — not the most recent second. Why? Because launch detection systems typically rely on altitude changes to identify liftoff. Including very recent data (which may already show movement) would confuse this logic and result in a bad zero point. Using the "2nd-to-last" second instead gives you a quieter, more accurate reference without interfering with launch detection.

You can enable or disable this feature at any time using:

```cpp
myBaro.setBiasCorrectionMode(true); // or false
```

And you should lock in the baseline permanently at liftoff by calling:

```cpp
myBaro.markLiftoff();
```

This disables further corrections and locks the AGL altitude reference point in place.

### **Accessing Raw Data**

The following methods are available for reading the latest values:

```cpp
virtual double getPressure() const; // hPa
virtual double getTemp() const; // Deg C
virtual double getTempF() const; //Deg F
virtual double getPressureAtm() const; // atm
virtual double getASLAltFt() const; // Above Sea Level - Ft
virtual double getASLAltM() const;  // Above Sea Level - M
virtual double getAGLAltM() const;  // Above Ground Level - M
virtual double getAGLAltFt() const; // Above Ground Level - Ft
```

**Above Ground Level** is either denoted as alt difference since boot up, or, if you are using bias Correction, difference since bias correction was disabled.

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
