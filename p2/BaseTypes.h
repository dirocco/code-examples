#pragma once

#include <stdint.h>
#include <stdio.h>
#include <ctype.h>

typedef uint32_t Price;
typedef uint32_t Quantity;

enum class Side : uint8_t
{
    bid = 1,
    ask = 0,
};

inline void toString(char* buf, size_t bufLen, Side s)
{
    snprintf(buf, bufLen, "%s", s == Side::bid ? "bid" : "ask");
}

union Symbol
{
    char s[8];
    uint64_t n;

    static inline Symbol itchify(const char* name)
    {
        Symbol out;
        int i = 0;
        for(; i < 8 && isalpha(name[i]); i++)
            out.s[i] = name[i];

        for(; i < 8; i++)
            out.s[i] = ' ';

        return out;
    }

    inline Symbol truncate() const
    {
        Symbol out;
        for(int i = 0; i < 8; i++)
        {
            if(s[i] == ' ')
            {
                out.s[i] = '\0';
                break;
            }
            else out.s[i] = s[i];
        }
        return out;
    }

    static constexpr Symbol None() { return (Symbol){ .n = 0 }; }
};
static_assert(sizeof(Symbol) == 8, "Symbol miscompiled");

inline void toString(char* buf, size_t bufLen, Symbol s)
{
    Symbol t = s.truncate();
    char* pos = t.s;
    while(*pos != '\0' && pos - t.s < bufLen)
        *buf++ = *pos++;
}

