#pragma once

#include "Logging.h"

#include <stdint.h>

template<typename Key, int prime>
struct primeHash
{
    uint32_t operator()(const Key& in) const
    {
        return in % prime;
    }
};


template<typename Key, int pow2>
struct pow2hash
{
    uint32_t operator()(const Key& in) const
    {
        return in & (pow2 - 1);
    }
};

class FlatHashable
{
public:
    bool m_initialized : 1;

    FlatHashable()
    {
        m_initialized = 1;
    }

    bool initialized()
    {
        return m_initialized == 1;
    }
} __attribute__((packed));

template<typename Key, typename Value, int NumElements, typename Hash = pow2hash<Key, 8192> >
class FlatHash
{
    Value m_backing[NumElements] __attribute__((aligned(64)));
    FlatHash& operator=(const FlatHash&) = delete;
    FlatHash(const FlatHash&) = delete;

public:
    FlatHash()
    {
        memset(m_backing, 0, NumElements * sizeof(Value));
    }

    template<class ... Args>
    bool initialize(const Key& k, Args ... args)
    {
        Key pos = Hash()(k);
        bool collision = false;
        while((unlikely(m_backing[pos].initialized() && !(m_backing[pos] == k) )))
        {
            collision = true;
            pos = (pos + 1) % NumElements;
        }

        if(m_backing[pos].initialized() && m_backing[pos] == k)
            return false;

        new(&m_backing[pos]) Value(args...);
        m_backing[pos].m_initialized = true;
        return collision;
    }

    Value* find(const Key& k)
    {
        Key pos = Hash()(k);
        while(m_backing[pos].initialized())
        {
            if(m_backing[pos] == k)
                return &m_backing[pos];

            pos = (pos + 1) % NumElements;
        };

        return NULL;
    }

    void erase(const Key& k)
    {
        Key pos = Hash()(k);
        do
        {
            if(likely(m_backing[pos] == k))
                m_backing[pos].m_initialized = false;
            pos = (pos + 1) % NumElements;
        } while(m_backing[pos].initialized());
    }

} __attribute((aligned(64)));;

