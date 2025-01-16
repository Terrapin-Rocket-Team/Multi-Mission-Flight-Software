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
    constexpr EventID operator"" _i(const char *str, size_t len)
    {
        return fnv1a_32(str, len);
    }

    // Forward-declare EventManager
    class EventManager;

    class Event
    {
        friend class EventManager;

    public:
        Event(EventID id) : ID(id) {}
        EventID ID;

    private:
        Event *next = nullptr;
    };

    class IEventListener
    {
        friend class EventManager;

    public:
        IEventListener();
        virtual ~IEventListener();

        virtual void onEvent(const Event *e) = 0;

    private:
        IEventListener *next = nullptr;
    };

    class EventManager
    {
    public:
        void subscribe(IEventListener *l);
        void unsubscribe(IEventListener *l);
        void invoke(const Event &e);

    private:
        IEventListener *firstLis = nullptr, *lastLis = nullptr;
        Event *firstEvent = nullptr, *lastEvent = nullptr;
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
        if (!firstLis)
        {
            firstLis = lastLis = l;
            return;
        }
        auto t = firstLis;
        while (t)
        {
            if (t == l)
                return;
            t = t->next;
        }
        lastLis->next = l;
        lastLis = l;
    }

    inline void EventManager::unsubscribe(IEventListener *l)
    {
        if (!l || !firstLis)
            return;
        if (firstLis == l)
        {
            firstLis = firstLis->next;
            if (!firstLis)
                lastLis = nullptr;
            return;
        }
        auto t = firstLis;
        while (t->next)
        {
            if (t->next == l)
            {
                t->next = t->next->next;
                if (!t->next)
                    lastLis = t;
                return;
            }
            t = t->next;
        }
    }

    inline void EventManager::invoke(const Event &e)
    {
        auto t = firstLis;
        while (t)
        {
            t->onEvent(&e);
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
