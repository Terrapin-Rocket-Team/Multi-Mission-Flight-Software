#ifndef CIRC_BUFFER_H
#define CIRC_BUFFER_H

template <typename T>
class CircBuffer
{
private:
    T *buffer;
    int size;
    int head;
    int tail;
    int count;

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