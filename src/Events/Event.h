#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>
#include <stddef.h>

/*
 * The hash method at the top of this file is a way to change string literals to integers at compile time,
 * reducing strcmp() calls during runtime. To use it, include this file and type "LITERAL"_i and it will be hashed to an int32.
 * All events need to use this method, but the end use case is very simple.
 * If two events seem to be doing the same thing, that means there may have been a hash collision. Try renaming one of the events.
 */

namespace mmfs
{
    using EventID = uint32_t;

    constexpr uint32_t fnv1a_32(const char *s, size_t count)
    {
        uint32_t hash = 2166136261u;
        for (size_t i = 0; i < count; i++)
        {
            hash ^= static_cast<unsigned char>(s[i]);
            hash *= 16777619u;
        }
        return hash;
    }
    constexpr uint32_t operator"" _i(const char *str, size_t len)
    {
        return fnv1a_32(str, len);
    }

    // Forward-declare EventManager
    class EventManager;


    class IEventListener
    {
        friend class EventManager;

    public:
        IEventListener();
        virtual ~IEventListener();

        virtual void onEvent(EventID eventID, const void *data) = 0;

    private:
        IEventListener *next = nullptr;
    };

    class EventManager
    {
    public:
        void subscribe(IEventListener *l);
        void unsubscribe(IEventListener *l);
        void invoke(EventID eventID, const void *data = nullptr);

    private:
        IEventListener *first = nullptr;
        IEventListener *last = nullptr;
    };

    EventManager &getEventManager();

    inline IEventListener::IEventListener()
    {
        getEventManager().subscribe(this);
    }
    inline IEventListener::~IEventListener()
    {
        getEventManager().unsubscribe(this);
    }

    inline void EventManager::subscribe(IEventListener *l)
    {
        if (!l)
            return;
        if (!first)
        {
            first = last = l;
            return;
        }
        auto t = first;
        while (t)
        {
            if (t == l)
                return;
            t = t->next;
        }
        last->next = l;
        last = l;
    }

    inline void EventManager::unsubscribe(IEventListener *l)
    {
        if (!l || !first)
            return;
        if (first == l)
        {
            first = first->next;
            if (!first)
                last = nullptr;
            return;
        }
        auto t = first;
        while (t->next)
        {
            if (t->next == l)
            {
                t->next = t->next->next;
                if (!t->next)
                    last = t;
                return;
            }
            t = t->next;
        }
    }

    inline void EventManager::invoke(EventID eventID, const void *data)
    {
        auto t = first;
        while (t)
        {
            t->onEvent(eventID, data);
            t = t->next;
        }
    }

    inline EventManager &getEventManager()
    {
        static EventManager em;
        return em;
    }
}
#endif
