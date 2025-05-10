# CircBuffer

This class is exactly what it sounds like. It uses an array based implementation of a queue to hold a buffer of any kind of object. Internally, it's used as part of the Sensor drift correction system to store data for averaging, but it can realistically be used for anything.

## Usage

Initialization and adding/removing:
```cpp
#include <CircBuffer.h>

// Create a circular buffer of 10 integers
CircBuffer<int> buffer(10);
// Add an element to the buffer
buffer.push(1);
// See top of the queue
int top = buffer.peek();
// Pop an element from the buffer
int popped = buffer.pop();
```

There are also these other standard functions, that do about what you'd expect:
```cpp
bool isEmpty();
bool isFull();
int getSize(); // capacity of buffer
int getCount(); // number of elements in buffer
void clear();
```

Finally, you can use the `[]` operator to access elements. `#!cpp buffer[0]` is the top of the queue, and `#!cpp buffer[buffer.getCount() - 1]` is the bottom.