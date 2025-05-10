# Math

## Matrix

The **Matrix** class is designed for use with Kalman filters. It is based on a simple array of doubles and supports most matrix operations, including addition, subtraction, multiplication, transposition, and inversion.

```cpp
#include <Math.h>

// Dynamically allocate memory for a 3x3 matrix
double* data = new double[3 * 3]{
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0
};

Matrix m1 = Matrix(3, 3, data);
// Perform matrix operations here...
```

!!! Warning
    The **Matrix** class takes ownership of the array passed through its constructor and only makes a shallow copy. This means that:
    - The array **must** be dynamically allocated (i.e., allocated on the heap) rather than on the stack.
    - You should **not** modify or delete the array after passing it to the **Matrix** constructor.

---

## Quaternion

Our **Quaternion** class is adapted from Adafruit's `imu` namespace. It is used by the IMU to store orientation data. It supports vector operations such as addition, subtraction, and multiplication, as well as quaternion-specific operations like quaternion multiplication and conversion to/from Euler angles.

```cpp
#include <Math.h>

// Create a quaternion with specific components (w, x, y, z)
Quaternion q1 = Quaternion(1.0, 0.0, 0.0, 0.0);

// Normalize the quaternion to ensure valid rotation data
q1.normalize();

// Convert to Euler angles (yaw, pitch, roll)
Vector<3> euler = q1.toEuler321();

// Perform other quaternion operations here...
```

---

## Vector

The **Vector** class is used to represent a mathematical vector (not to be confused with the C++ `std::vector`). This **Vector** has a fixed length containing `double` values. It supports vector operations like addition, subtraction, multiplication, as well as dot products and cross products. It is also adapted from Adafruit's `imu` vector class.

```cpp
#include <Math.h>

// Create two 3D vectors
Vector<3> v1 = Vector(1.0, 2.0, 3.0);
Vector<3> v2 = Vector(3.0, 2.0, 1.0);

// Compute the dot product
double dotVal = v1.dot(v2); // 1*3 + 2*2 + 3*1 = 3 + 4 + 3 = 10

// Compute the cross product
Vector crossVal = v1.cross(v2); // (2*1 - 3*2, 3*3 - 1*1, 1*2 - 2*3)

// Perform additional vector operations as needed...
```
