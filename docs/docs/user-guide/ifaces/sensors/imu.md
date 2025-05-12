# IMU

The `IMU` class in MMFS represents an advanced inertial sensor interface designed to support both raw sensor reporting and fused orientation estimation. It processes data from gyroscopes, accelerometers, and optionally magnetometers, and internally runs a quaternion-based complementary filter to estimate device orientation.

Unlike basic sensors, IMUs operate in a multi-frame context — reporting both local-frame measurements and global-frame acceleration — and may expose both raw and filtered signals. Orientation is represented using quaternions but is often interpreted in Euler angles for ease of telemetry.

---

## **Overview**

An IMU in MMFS provides:

* Angular velocity (from gyroscope)
* Linear acceleration (from accelerometer)
* Magnetic field strength (from magnetometer)
* Quaternion orientation (internally computed)
* Gravity-compensated acceleration (global frame)

---

## **Inheritance Structure**

`IMU` inherits from [`Sensor`](sensor.md) and expects the user to override two core methods:

```cpp
bool init() override;
void read() override;
```

* `init()` sets up the hardware driver(s), configures sensors, and prepares communication.
* `read()` must populate the following fields with current measurements:

  * `measuredAcc` — linear acceleration in body frame
  * `measuredGyro` — angular velocity in deg/s
  * `measuredMag` — magnetic field vector (optional)

The class handles filtering and fusion internally during `update()`.

---

## **Orientation Fusion**

The `IMU` class performs sensor fusion using a quaternion-based complementary filter. This combines fast-but-drifting gyro data with stable-but-noisy accelerometer estimates of gravity direction. Magnetometers can further improve yaw stability.

The computed orientation is stored in:

```cpp
Quaternion orientation;
```

Global-frame acceleration (i.e. gravity-compensated) is computed using:

```cpp
Vector<3> getAccelerationGlobal();
```

and is derived by rotating the measured acceleration vector into the world frame.

---

## **Exposed Data Columns**

The IMU automatically exposes the following:

* `accX`, `accY`, `accZ` — linear acceleration (m/s²)
* `gyroX`, `gyroY`, `gyroZ` — angular velocity (deg/s)
* `magX`, `magY`, `magZ` — magnetic field (μT)
* `oriX`, `oriY`, `oriZ`, `oriW` — quaternion orientation

These are registered for logging and telemetry via `DataReporter` and packed for transmission.

---

## **Bias Correction Mode**

Bias correction enables zeroing gyro and acceleration offsets over time. If enabled before flight, the IMU will average recent values to zero-out drift while stationary.

```cpp
imu.setBiasCorrectionMode(true);
imu.markLiftoff(); // Locks in calibration at launch
```

Avoid enabling this while the vehicle is moving.

---

## **Usage Flow**

/// tab | Using an IMU

```cpp
MyIMU imu;
imu.begin();

loop() {
    imu.update();
    Quaternion q = imu.getOrientation();
    Vector<3> a_global = imu.getAccelerationGlobal();
}
```

///

/// tab | Implementing a Custom IMU

```cpp
class MyIMU : public IMU {
    MyIMULib hw;

    bool init() override {
        return hw.begin();
    }

    void read() override {
        measuredAcc = { hw.ax(), hw.ay(), hw.az() };
        measuredGyro = { hw.gx(), hw.gy(), hw.gz() };
        measuredMag = { hw.mx(), hw.my(), hw.mz() }; // optional
    }
};
```

///

---

## **Built-in Implementations**

### **[BMI088 + LIS3MDL](bmi088_lis3mdl.md)**

* Combines separate accelerometer, gyro, and magnetometer
* Uses MMFS’s internal filter and fusion logic
* Best for flexible, high-performance inertial applications

### **[BNO055](bno055.md)**

* Integrated 9DOF sensor with onboard fusion
* MMFS defers to the sensor’s internal orientation logic
* Returns pre-filtered quaternion and acceleration data

---

## **Advanced Controls**

The following parameters can be adjusted to control noise filtering sensitivity during static startup:

```cpp
imu.setAccelBestFilteringAtStatic(val); // threshold for accel stability
imu.setMagBestFilteringAtStatic(val);   // threshold for mag stability
```

Use these to tune how long the IMU waits before accepting a bias-corrected origin.

---

## **Summary**

* Implements `Sensor` with IMU-specific orientation logic
* Expects raw accel, gyro, and optional mag readings
* Computes quaternion orientation using a complementary filter
* Supports packed telemetry and gravity-compensated acceleration
* Used directly or as a base class for sensor-specific drivers

---

Written by ChatGPT. May not be fully accurate; verify against source.
