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
    };

    class IEventListener
    {
        friend class EventManager;

    public:
        IEventListener();
        virtual ~IEventListener();

        virtual void onEvent(const Event *e) = 0;
    };
    struct Listener
    {
        union Lis
        {
            void (*func)(const Event *);
            IEventListener *l;
        } ptr;
        bool isClass;
        Listener *next = nullptr;
        Listener(IEventListener *lis)
        {
            ptr.l = lis;
            isClass = true;
        }
        Listener(void (*func)(const Event *))
        {
            ptr.func = func;
            isClass = false;
        }
    };

    class EventManager
    {
    public:
        void subscribe(IEventListener *l);
        void subscribe(void (*func)(const Event *));
        void unsubscribe(IEventListener *l);
        void unsubscribe(void (*func)(const Event *));
        void invoke(const Event &e);

    private:
        Listener *firstLis = nullptr, *lastLis = nullptr;
        bool sub(Listener *l);
        bool unsub(Listener *l);
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
    inline bool EventManager::sub(Listener *l)
    {
        if (!l)
            return false;
        auto t = firstLis;
        if (!t)
        {
            firstLis = lastLis = l;
            return true;
        }
        while (t)
        {
            if (t->isClass && l->isClass)
            {
                if (t->ptr.l == l->ptr.l)
                    return false;
            }
            else if (!t->isClass && !l->isClass)
            {
                if (t->ptr.func == l->ptr.func)
                    return false;
            }
            t = t->next;
        }
        lastLis->next = l;
        lastLis = l;
        return true;
    }
    inline void EventManager::subscribe(IEventListener *l)
    {
        Listener *lis = new Listener(l);
        if (!sub(lis))
            delete lis;
    }
    inline void EventManager::subscribe(void (*func)(const Event *))
    {
        Listener *lis = new Listener(func);
        if (!sub(lis))
            delete lis;
    }

    inline bool EventManager::unsub(Listener *l)
    {
        if (!l || !firstLis)
            return false;

        auto t = firstLis;
        if ((firstLis->isClass && l->isClass && firstLis->ptr.l == l->ptr.l) ||
            (!firstLis->isClass && !l->isClass && firstLis->ptr.func == l->ptr.func))
        {
            firstLis = firstLis->next;
            if (!firstLis)
                lastLis = nullptr;
            delete t;
            return true;
        }
        while (t->next)
        {
            if ((t->next->isClass && l->isClass && t->next->ptr.l == l->ptr.l) ||
                (!t->next->isClass && !l->isClass && t->next->ptr.func == l->ptr.func))
            {
                auto t2 = t->next;
                t->next = t->next->next;
                if (!t->next)
                    lastLis = t;
                delete t2;
                return true;
            }
            t = t->next;
        }
        return false;
    }
    inline void EventManager::unsubscribe(IEventListener *l)
    {
        Listener *lis = new Listener(l);
        unsub(lis);
        delete lis;
    }
    inline void EventManager::unsubscribe(void (*func)(const Event *))
    {
        Listener *lis = new Listener(func);
        unsub(lis);
        delete lis;
    }

    inline void EventManager::invoke(const Event &e)
    {
        auto t = firstLis;
        while (t)
        {
            if (t->isClass)
                t->ptr.l->onEvent(&e);
            else
                t->ptr.func(&e);
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
