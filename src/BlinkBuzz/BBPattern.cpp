#include "BBPattern.h"

using namespace mmfs;
// Constructor
BBPattern::BBPattern() : head(nullptr), tail(nullptr) {}

BBPattern::BBPattern(int duration, int times, int pause)
    : head(nullptr), tail(nullptr)
{
    for (int i = 0; i < times; i++)
    {
        a(duration);
        a(pause == -1 ? duration : pause); // Add pause between each action, except the last one
    }
}

// Destructor: clean up linked list
BBPattern::~BBPattern()
{
    BBPatternNode *cur = head;
    while (cur != nullptr)
    {
        BBPatternNode *next = cur->next;
        delete cur;
        cur = next;
    }
}

// Add a duration to the pattern
BBPattern &BBPattern::a(int duration)
{
    BBPatternNode *newNode = new BBPatternNode(duration); // Dynamically allocate a new node
    if (head == nullptr)
    {
        head = newNode;
        tail = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }
    return *this; // Enable chaining
}

// Append another pattern to the current one
BBPattern &BBPattern::a(BBPattern pattern)
{
    BBPatternNode *cur = pattern.head;
    while (cur != nullptr)
    {
        a(cur->duration); // Add each node's duration to this pattern
        cur = cur->next;
    }
    return *this;
}

// Rest at end of pattern for duration
BBPattern &BBPattern::r(int duration)
{
    tail->duration = duration;
    return *this;
}

// BBPatternNode constructor
BBPattern::BBPatternNode::BBPatternNode(int duration)
    : duration(duration), next(nullptr) {}

// Copy constructor (deep copy)
BBPattern::BBPattern(const BBPattern &other)
{
    head = nullptr;
    tail = nullptr;
    BBPatternNode *cur = other.head;
    while (cur != nullptr)
    {
        a(cur->duration); // Copy each node
        cur = cur->next;
    }
}

// Assignment operator (deep copy)
BBPattern &BBPattern::operator=(const BBPattern &other)
{
    if (this == &other)
        return *this; // Handle self-assignment

    // First, clear the current list
    this->~BBPattern();

    // Then, copy the other list
    BBPatternNode *cur = other.head;
    while (cur != nullptr)
    {
        a(cur->duration); // Add each node's duration to this pattern
        cur = cur->next;
    }
    return *this;
}
