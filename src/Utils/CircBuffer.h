#ifndef CIRC_BUFFER_H
#define CIRC_BUFFER_H

/*

A Queue implementation using a circular buffer. Can use [] operator to peek at elements. (e.g. buffer[0] to get the oldest element, buffer[buffer.getCount() - 1] to get the newest element)

Intended usage: A buffer storing a few seconds of sensor data that can be averaged over to provide a more accurate "rest" value for the sensor.

Example usage from Barometer:

            pressureBuffer.push(pressure);

            double sum = 0;

            // Counts the first half othe buffer to avoid including potential launch transients before launch is detected.
            int valsToCount = std::min(pressureBuffer.getCount(), CIRC_BUFFER_LENGTH - CIRC_BUFFER_IGNORE);
            
            for (int i = 0; i < valsToCount; i++)
            {
                sum += pressureBuffer[i]; // [] operator
            }
            groundPressure = sum / valsToCount / 1.0;
            groundAltitude = calcAltitude(groundPressure);
        }
*/

template <typename T>
class CircBuffer
{
private:
    T *buffer;
    int size = 0;
    int head = 0;
    int tail = 0;
    int count = 0;

public:
    CircBuffer(int size);
    ~CircBuffer();
    void push(T item);
    T pop();
    T peek();
    int getCount();
    int getSize();
    bool isFull();
    bool isEmpty();
    void clear();

    T &operator[](int index) { return buffer[(tail + index) % size]; }
    T operator[](int index) const { return buffer[(tail + index) % size]; }
};

template <typename T>
CircBuffer<T>::CircBuffer(int size)
{
    this->size = size;
    buffer = new T[size];
    clear();
}

template <typename T>
CircBuffer<T>::~CircBuffer()
{
    delete[] buffer;
}

template <typename T>
void CircBuffer<T>::push(T item)
{
    if (isFull())
    {
        tail = (tail + 1) % size;
        count--;
    }
    buffer[head] = item;
    head = (head + 1) % size;
    count++;
}

template <typename T>
T CircBuffer<T>::pop()
{
    if (isEmpty())
    {
        return T();
    }
    T item = buffer[tail];
    tail = (tail + 1) % size;
    count--;
    return item;
}

template <typename T>
T CircBuffer<T>::peek()
{
    if (isEmpty())
    {
        return T();
    }
    return buffer[tail];
}

template <typename T>
int CircBuffer<T>::getCount()
{
    return count;
}

template <typename T>
int CircBuffer<T>::getSize()
{
    return size;
}

template <typename T>
bool CircBuffer<T>::isFull()
{
    return count == size;
}

template <typename T>
bool CircBuffer<T>::isEmpty()
{
    return count == 0;
}

template <typename T>
void CircBuffer<T>::clear()
{
    head = 0;
    tail = 0;
    count = 0;
}

#endif