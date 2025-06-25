# GPS

The `GPS` class in MMFS provides a standard interface for satellite-based positioning modules. Like all sensors in MMFS, it inherits from [`Sensor`](sensor.md), allowing it to integrate directly with the system’s data reporting and state machine infrastructure. However, it also offers specific features like positional tracking, fix quality monitoring, and displacement-from-origin calculations.

---

## **Overview**

GPS modules output geographic location, altitude, speed, and timing information. The MMFS `GPS` class manages parsing, tracking fix quality, and computing displacement from the original location, so you don’t have to.

---

## **Inheritance Structure**

`GPS` inherits from [`Sensor`](sensor.md), so it exposes `begin()` and `update()` as part of its standard API:

* `begin()` → Initializes the module, sets the origin once a valid fix is obtained
* `update()` → Retrieves the latest fix and updates all internal values

You only need to override two methods:

```cpp
bool init() override;
bool read() override;
```

* `init()` should configure the underlying hardware (e.g., set baud rate, GPS mode, power state)
* `read()` should parse data from the GPS module and update the internal position, heading, and fix fields

This mirrors the behavior seen in other sensors like [`Barometer`](barometer.md).

---

## **Exposed Data Columns**

The `GPS` class exposes the following telemetry columns:

* **Latitude** (`lat`) – degrees
* **Longitude** (`lon`) – degrees
* **Altitude** (`alt`) – meters
* **Fix Quality** (`fix`) – number of connected satellites (or equivalent measure)
* **Heading** (`head`) – degrees

These columns are registered using the `DataReporter` system and appear automatically in logs and telemetry.

---

## **Displacement and Origin Tracking**

The GPS class tracks three core positional vectors:

* `position` – the current fix (lat, lon, alt)
* `origin` – the first valid fix after boot (lat, lon, alt)
* `displacement` – the relative XYZ distance (in meters) between current position and origin

This makes it easy to track launch site-relative movement without needing external references.

!!! note
    The distance calculation is taken from [this](https://blog.mapbox.com/fast-geodesic-approximations-with-cheap-ruler-106f229ad016) article and [this](https://github.com/mapbox/cheap-ruler/blob/main/index.js#L475) repo. As I (Drew) understand it, it's an accurate approximation of the Vincenty formulae to find the distance between two points on the earth.

---

## **Time and Heading Utilities**

MMFS GPS also tracks the time-of-day from the satellite fix. You can access it with:

```cpp
const char *getTimeOfDay() const;
```

It also computes the travel heading in degrees:

```cpp
double getHeading() const;
```

This is generally based on the change in GPS position across updates.

---

## **Usage Flow**

/// tab | Using a GPS Module

If you're using an MMFS-supported GPS driver:

```cpp
MyGPS gps;
gps.begin();

loop() {
    gps.update();
    if (gps.getHasFirstFix()) {
        Vector<3> pos = gps.getPos();
        Vector<3> disp = gps.getDisplacement();
    }
}
```

GPS fixes typically take a few seconds to stabilize. Use `getHasFirstFix()` to avoid reading garbage data.

///

/// tab | Implementing Your Own GPS Driver

To add a new GPS module:

1. **Find a library** that parses NMEA or binary output from your hardware
2. **Create a new class derived from `GPS`**:

    ```cpp
    class MyGPS : public GPS {
        SomeLibGPS hw;
    public:
        bool init() override {
            hw.begin(9600);
            return true;
        }
        bool read() override {
            hw.read();
            if (hw.hasFix()) {
                position = { hw.latitude(), hw.longitude(), hw.altitude() };
                fixQual = hw.fixQuality();
                heading = hw.headingDegrees();
            }
        }
    };
    ```

3. **Call `begin()` and `update()`** as in the usage example.

///

---

## **Advanced Features**

### **Using Bias Correction Mode**

Bias correction mode in the GPS module serves the same core purpose as it does for the barometer: stabilizing the baseline reference — in this case, your origin point (lat, lon, alt). Instead of relying on a single fix, it smooths out noise and minor drift by averaging over several recent GPS readings.

By default, the origin is set the first time a valid fix is obtained. If bias correction is enabled, MMFS continues updating this origin using the second-to-last second of position data. This helps reduce the effect of GPS jitter or slow drift while the rocket is sitting still.

Why not use the most recent second? Same reason as with pressure sensors — the moment just before launch may already contain motion. Locking in a slightly older, averaged position provides a cleaner and safer ground reference for computing AGL altitude or displacement.

To toggle this feature:

```cpp
myGps.setBiasCorrectionMode(true);  // enable smoothing
myGps.setBiasCorrectionMode(false); // freeze origin
```
And once you're confident the rocket has launched or the vehicle is moving, lock in the reference point permanently:

```cpp
myGps.markLiftoff();
```

From that point onward, the origin stays fixed and all displacement values are computed relative to it.

---

## **Summary**

* `GPS` standardizes satellite position data in MMFS
* You only need to implement `init()` and `read()` to add new modules
* Outputs latitude, longitude, altitude, fix quality, and heading
* Tracks local displacement and satellite time-of-day
* Integrates with MMFS telemetry/logging and supports packed data

---

Written by ChatGPT. May not be fully accurate; verify against source.
