# DataReporter

The **DataReporter** class is used by the **Logger** system in MMFS to log flight data from various sources. Each data reporter defines the variables to be recorded, the variable types, and the labels that appear in the output CSV file. This allows you to separate your flight data into multiple, self-contained objects that easily integrate with MMFS.

---

## **Key Concepts**

2. **Column Definition**  
   In your **DataReporter** subclass or instance, you’ll define columns inside its constructor. You specify:
    - The **variable** to record (by passing a pointer to it).
    - The **type** of that variable (through the `PackedType` enum).
    - The **label** to appear in the CSV header.

3. **Name and Uniqueness**  
   **DataReporter** objects can be given names in their constructors. If you do not specify a name, a unique one will be assigned using a static counter.

4. **Removal of Unneeded Columns**  
   If you extend another **DataReporter** but want to remove one of its default columns, you can call `removeColumn(...)` in your subclass constructor or elsewhere.

5. **Auto-Packing**  
    Data reporters will automatically package the data, but they do not come with an update step. You must write your own update step to update the data.
---

## **Usage Overview**

Below is an example of how to create and use a custom **DataReporter**:

```cpp
#include <DataReporter.h>

using namespace mmfs;

class MyCustomReporter : public DataReporter {
public:
    // Constructor with an optional name
    MyCustomReporter(const char* name = "MyReporter") 
        : DataReporter(name) 
    {
        // Example variables to record
        int altitude;        // You'd likely store this somewhere else in real code
        float velocity;
        double latitude; 

        // In the constructor, define the columns you want logged:
        // addColumn(PackedType, pointerToVar, "Label in CSV")
        addColumn(INT, &altitude, "Altitude");
        addColumn(FLOAT, &velocity, "Velocity");
        addColumn(DOUBLE_HP, &latitude, "Latitude"); // (1)!
    }

    // Add your own update step here to modify the data.
};
```
{.annotate}

1. Use `DOUBLE` for 3 decimal point precision, or `DOUBLE_HP` if you want 7 points of precision. In this example, 3 decimal points of latitude is only accurate to about 100 meters, so we record all 7 for better location precision.

### **Adding Columns**

To add columns, call `addColumn()` or `insertColumn()` in your **DataReporter** constructor (or initialization method):

- **`#!cpp addColumn(PackedType t, T* variable, const char* label)`**  
  Appends a new column with the given type, pointer to a variable, and the label that appears in the CSV header.  

- **`#!cpp insertColumn(uint8_t place, PackedType t, T* variable, const char* label)`**  
  Inserts a new column at a specific index.

### **Removing Columns**

If you inherit from another **DataReporter** but do not want one of its columns, call:

```cpp
removeColumn("UnwantedColumnLabel");
```

This removes the column with label `"UnwantedColumnLabel"` from your final data set.

### **Default Behavior**

- When you create a **DataReporter** without a name, it automatically assigns a unique name based on a static counter.

---

## **Step-by-Step Example**

Here’s a simple step-by-step guide for beginners:

1. **Create a custom reporter**  
   Derive a new class from **DataReporter**. In your constructor, add the columns you want to record, with pointers to the variables that hold that data.

2. **Register it with MMFS**  
   Pass your **DataReporter** object to the `MMFSConfig` object (via `withOtherDataReporters(...)`) or manually to the **Logger** if you’re using a standalone approach.

3. **Let MMFS manage updates**  
   Whenever `MMFSSystem::update()` (or an equivalent mechanism) is called, your reporter’s most recent data is automatically logged as a CSV row. Keep in mind that MMFS and **DataReporter** do *not* update the data stored in those variables.

4. **Check CSV output**  
   After a flight (or test run), retrieve the `.csv` file and confirm your new columns (Altitude, Velocity, etc.) appear as expected with the correct data.

---

## **Summary**

- **DataReporter** objects act as modular data sources for logging in MMFS.
- You specify a name, define columns for logging, and let **MMFS** handle how and when the data is stored or transmitted.

Use this class to easily add flight data logging to your project, keeping your code organized and your flight logs detailed.

---

Written by ChatGPT. Information may not be completely accurate.