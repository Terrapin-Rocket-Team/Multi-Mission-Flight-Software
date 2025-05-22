# Encoder

The `Encoder_MMFS` class provides a standard MMFS interface for rotary or linear encoders. It inherits from [`Sensor`](sensor.md) and implements the familiar MMFS lifecycle (`begin()`, `update()`), while requiring only simple hardware-specific methods to be overridden.

---

## **Overview**

The encoder tracks step counts — ticks of a shaft or linear scale — relative to its startup position. It is commonly used for measuring angular rotation, wheel travel, or simple mechanical motion.

Internally, the `Encoder_MMFS` class handles lifecycle integration and telemetry exposure. You only need to implement two functions:

```cpp
bool init() override;
void read() override;
```

* `init()` sets up your encoder hardware (e.g., pin modes, timers, interrupts)
* `read()` fetches the current number of steps and updates internal variables

---

## **Inheritance Structure**

`Encoder_MMFS` inherits from [`Sensor`](sensor.md), and therefore implements:

* `begin()` → Calls `init()` and stores whether bias correction is enabled
* `update()` → Calls `read()` and then updates the telemetry data buffer

This ensures your encoder integrates seamlessly with MMFSSystem and the logging stack.

---

## **Exposed Data Columns**

The encoder automatically registers the following telemetry column:

* **Steps** (`Rel Steps`) – integer number of encoder steps since initialization

These are reported through the [`DataReporter`](sensor.md#datareporter-integration) API and appear in logs and telemetry streams.

---

## **Usage Flow**

/// tab | Using an Encoder

```cpp
MyEncoder encoder;
encoder.begin();

loop() {
    encoder.update();
    int ticks = encoder.getSteps();
}
```

///

/// tab | Implementing a Custom Encoder

To add a hardware-specific encoder driver, subclass `Encoder_MMFS` and implement `init()` and `read()`:

```cpp
class MyEncoder : public Encoder_MMFS {
    MyHardwareEncoder hw;

    bool init() override {
        return hw.begin(); // or whatever init call the library uses
    }

    void read() override {
        currentRelativeSteps = hw.getSteps();
    }
};
```

///

---

## **Summary**

* `Encoder_MMFS` provides a standard interface for step-counting encoders
* Override `init()` and `read()` to integrate your own hardware
* Automatically hooks into MMFS telemetry and logging
* Supports packed telemetry for bandwidth-sensitive applications

---

Written by ChatGPT. May not be fully accurate; verify against source.
