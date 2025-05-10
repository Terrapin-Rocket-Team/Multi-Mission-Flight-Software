# Events

Events in MMFS allow you to respond to system-wide actions or notifications. By default, these events are used to coordinate various parts of the library (e.g., logging, GPS, state changes). You can also create your own events or custom listeners to handle specific needs.

---

## **Core Components**

1. **`Event`**  
    A simple class carrying an **EventID**. Developers can create new event types by inheriting from `Event` and adding any additional data they want to share with listeners.

2. **`IEventListener`**  
    An interface that you can inherit from to automatically subscribe to **all** events in MMFS. Each `IEventListener` must implement:
    ```cpp
    void onEvent(const Event *e) override;
    ```
    Inside this function, you decide how to handle any event passed to you.

3. **`EventManager`**  
    The main system responsible for event dispatch. It tracks all active `IEventListener` objects and calls their `onEvent()` methods whenever an event is invoked.

---

## **How It Works**

1. **Event Creation**  
    An event in MMFS has a unique numeric ID. You can either:
    - Directly specify an integer ID, or
    - Use the handy compile-time hashing of a string literal with `"SOME_TEXT"_i`.

   For instance:
   ```cpp
   // Produces a unique integer ID at compile time
   static const EventID MY_CUSTOM_EVENT = "MY_CUSTOM_EVENT"_i;
   ```

2. **Subscribing to Events**  
   Whenever you create a class that inherits from `IEventListener`, its constructor automatically subscribes you to the global event manager. You do not need to manually register anything, unless you want to manually manage subscription/unsubscription.

   You may also manually subscribe/unsubscribe specific global functions (**not class methods**) by calling

   ```cpp
   getEventManager().subscribe(funcName);
   ```

3. **Dispatching Events**  
   Any code can invoke an event by calling:
   ```cpp
   getEventManager().invoke(yourEventObject);
   ```
   All active `IEventListener` objects will receive this event through their `onEvent()` method.

4. **Handling Events**  
   Since **every** listener hears **every** event, you typically use an `if` or `switch` statement based on the event’s ID, or perform a dynamic cast if you’re dealing with a custom event type. For example:
   ```cpp
   void onEvent(const Event *e) override {
       switch (e->ID) {
           case "LOG_DATA"_i:
               // Handle log data event
               break;

           case "GPS_FIX"_i:
               // Handle GPS fix event
               break;

           default:
               // Ignore or handle other events
               break;
       }
   }
   ```

---

## **Default Events**

MMFS provides a few built-in events for common scenarios:

- **`GPSFix`**  
  Notifies listeners when a GPS fix occurs or changes. It carries a pointer to the `GPS` object and a boolean indicating if this is the first time a fix has been acquired since boot.

- **`LogData`**  
  Fired when new log data is recorded. Carries information about the log destination (`SD` vs. `USB`), the log type (e.g., `INFO_`, `ERROR_`), and the actual string message.

- **`BoolEvent`**  
  A simple true/false style event used for various system checks (e.g., initialization success).

---

## **Default Event Handler**

**`DefaultEventHandler`** is a built-in class that inherits from `IEventListener` and implements `onEvent()`. It provides out-of-the-box handling for:

- GPS fixes
- Log data notifications
- Initialization events

If you need to change how these events are processed, you can subclass `DefaultEventHandler` and override its methods:
```cpp
void onEvent(const Event *event) override {
    // ...
    // or call the parent to handle some events, and handle others yourself
    DefaultEventHandler::onEvent(event);
}
```

If you **do not** want the default behavior at all, you can remove the default event listener by using:
```cpp
MMFSConfig config = MMFSConfig()
    .withNoDefaultEventListener();
```
This way, only your custom listeners will process events.

---

## **Typical Usage Example**

Below is a simple example showing how you might create a custom event, a custom listener, and dispatch an event:

```cpp
#include <Event.h>          // Contains IEventListener, EventManager, etc.
#include <DefaultEvents.h>  // For reference to built-in events (optional)

using namespace mmfs;

// 1. Define a custom event by inheriting from Event
class MyCustomEvent : public Event {
public:
    MyCustomEvent(const char* message)
      : Event("MY_CUSTOM_EVENT"_i), msg(message) {}

    const char* msg;
};

// 2. Create a listener to handle events
class MyListener : public IEventListener {
public:
    void onEvent(const Event *e) override {
        if (e->ID == "MY_CUSTOM_EVENT"_i) {
            const MyCustomEvent* ce = static_cast<const MyCustomEvent*>(e);
            // Do something with ce->msg
        }
        else if (e->ID == "LOG_DATA"_i) {
            // Possibly handle built-in LogData event
        }
        // Other events...
    }
};

void setup() {
    // The MyListener constructor automatically subscribes to the event manager
    static MyListener myListener;

    // Dispatch an event
    MyCustomEvent evt("Hello World!");
    getEventManager().invoke(evt);
}

void loop() {
    // ...
}
```

1. **`MyCustomEvent`** extends `Event`, adding `msg` to store a string.  
2. **`MyListener`** overrides `onEvent()`, checking the event ID to see if it’s our custom event.  
3. In `setup()`, we create a `MyListener` instance (thus subscribing) and invoke an event.

---

## **Summary**

- **Events** allow a decoupled way of communicating changes or notifications within MMFS.  
- **`IEventListener`** classes automatically subscribe to all events.  
- **Event IDs** can be manually specified or derived via the string hashing syntax `"SOME_ID"_i`.  
- **Default events** like `LogData`, `GPSFix`, and `BoolEvent` are provided for common tasks.  
- **`DefaultEventHandler`** gives out-of-the-box functionality that you can override or remove (`.withNoDefaultEventListener()`) if you want full control.
- You can manually subscribe specific methods to the EventManager.

By taking advantage of this flexible event system, your application can remain modular and easy to maintain, with different parts of the system responding only to the events they care about.

---

Written by ChatGPT. Information may not be completely accurate.