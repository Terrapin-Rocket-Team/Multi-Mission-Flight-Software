# State

The `State` class in MMFS serves as the central data aggregator, coordinating all sensors, optionally fusing data via a filter (e.g. a Kalman filter), and determining the current flight stage. This class is typically passed into `MMFSSystem` for high-level control of the flight software.

---

## **Overview**

The `State` class is responsible for:

1. **Sensor updates** – polling and managing all registered sensors
2. **Sensor fusion (optional)** – using a provided `Filter` to compute position/velocity/acceleration
3. **Flight stage logic** – calling a user-implemented `determineStage()` to manage the rocket’s flight state machine

---

## **Expected Usage**

You are expected to subclass `State` and implement:

```cpp
void determineStage() override;
```

This method determines the current flight phase (e.g., pad, boost, coast, recovery) based on sensor values like altitude, acceleration, or velocity.

In your subclass, you'll typically query sensors like so:

```cpp
auto baro = reinterpret_cast<Barometer *>(getSensor(BAROMETER_));
auto imu = reinterpret_cast<IMU *>(getSensor(IMU_));
float alt = baro ? baro->getAGLAltM() : 0;
```

---

## **Constructor**

```cpp
State(Sensor **sensors, int numSensors, Filter *filter);
```

* `sensors`: an array of pointers to various Sensor-derived classes (e.g. GPS, IMU, Barometer)
* `numSensors`: number of sensors in the array
* `filter`: optional pointer to a `Filter` object for data fusion (e.g. Kalman filter)

---

## **Lifecycle and Core Methods**

### **Initialization**

```cpp
bool init(bool useBiasCorrection = false);
```

Initializes all valid sensors and optionally enables bias correction. Failing sensors are disabled and logged.

### **State Update**

```cpp
void updateState(double currentTime = -1);
```

Called once per loop. This does the following:

1. Records `lastTime` and updates `currentTime`
2. Calls `updateSensors()` on all valid sensors
3. Calls `updateKF()` or `updateWithoutKF()` depending on filter presence
4. Updates orientation, heading, and positional fields
5. Finally calls `determineStage()` (which you override)

---

## **Sensor Access**

Sensors can be accessed by type using:

```cpp
Sensor *getSensor(SensorType type, int sensorNum = 1);
```

Returned sensors can then be downcasted to the appropriate type (e.g. `Barometer`, `GPS`).

---

## **Internal State Variables**

These are updated every loop and can be queried with `get*()` methods:

* `Vector<3> position` — Displacement in meters
* `Vector<3> velocity` — m/s
* `Vector<3> acceleration` — m/s²
* `Quaternion orientation` — current rotation from IMU
* `Vector<3> eulerAngles` — roll, pitch, yaw (derived)
* `Vector<2> coordinates` — lat/lon (from GPS)
* `float heading` — direction of travel
* `int stage` — user-defined flight stage value

These are all exposed via `DataReporter` for logging and telemetry.

---

## **Kalman Filter Integration**

If a `Filter` is passed into the constructor:

* It will be initialized during `init()`
* Called on each `updateState()` to fuse sensor data
* Internal `stateVars` will be allocated to store the filter’s output

You must ensure your `Filter` subclass implements the expected interface (e.g., `initialize()`, `update()`, etc.).

---

## **Example Implementation**

```cpp
class MyAvionicsState : public State {
public:
    MyAvionicsState(Sensor **s, int n, Filter *f) : State(s, n, f) {}

    void determineStage() override {
        IMU *imu = reinterpret_cast<IMU *>(getSensor(IMU_));
        Barometer *baro = reinterpret_cast<Barometer *>(getSensor(BAROMETER_));

        if (stage == 0 &&
            (sensorOK(imu) ? abs(imu->getAccelerationGlobal().z()) > 25 : false) ||
            (sensorOK(baro) ? baro->getAGLAltFt() > 60 : false)) {

            getLogger().setRecordMode(FLIGHT);
            stage = 1;
            timeOfLaunch = currentTime;
            getLogger().recordLogData(INFO_, "Launch detected.");
        }
    }
};
```

This shows a basic launch detection using either IMU acceleration or barometric altitude.

---

## **Integration with MMFSSystem**

Once your `State` is initialized, pass it to MMFSSystem like so:

```cpp
MyAvionicsState state(sensors, numSensors, new MyKalmanFilter());
MMFSConfig config = MMFSConfig()
                .withState(&state)
                // any other config options
                ;

```

MMFSSystem will handle calling `updateState()` and routing stage transitions.

---

## **Summary**

* `State` centralizes sensor access, fusion, and stage tracking
* You override `determineStage()` to define flight logic
* Sensors are polled and filtered each update cycle
* Internal state is auto-logged via `DataReporter`
* Integrates directly with `MMFSSystem`

---

Written by ChatGPT. Information may not be accurate.
