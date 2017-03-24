#pragma once

#include "BucketTimer.h"
#include "Logging.h"

#include <ctype.h>
#include <stdint.h>
#include <algorithm>
#include <string.h>

//#define USE_STDLIB

template<typename IntType = int64_t>
IntType atoi(const char* buf)
{
    TIMER_START(Atoi, 1024);

#ifdef USE_STDLIB
    return strtoul(buf, NULL, 10);
#else
    while(isspace(*buf)) buf++;

    IntType i = 0;
    bool isNegative = false;
    if(buf[0] == '-')
    {
        isNegative = true;
        buf++;
    }

    while(isdigit(*buf))
    {
        i *= 10;
        i += *buf - '0';
        buf++;
    }

    return isNegative ? -i : i;
#endif

    TIMER_STOP(Atoi);
}

inline int64_t atoi64(const char* buf) { return atoi<int64_t>(buf); }

template<typename IntType = int64_t>
size_t printInt(IntType n, char* buf)
{
#if 1
    if(unlikely(n == 0x8000000000000000))
    {
        memcpy(buf, "-9223372036854775808", 20);
        return 20;
    }
#endif

    unsigned pos = 0;
    unsigned isNegative = 0;
    if(n < 0)
    {
        buf[pos++] = '-';
        n = -n;
        isNegative = 1;
    }

    do
    {
        buf[pos++] = (n % 10) + '0';
        n /= 10;
    } while(n > 0);

    char* front = buf + isNegative;
    char* back = buf + pos - 1;
    while(front < back)
        std::swap(*front++, *back--);

    return pos;
}

template<typename IntType = int64_t>
size_t printPair(char* buf, IntType n1, IntType n2)
{
    size_t pos = printInt(n1, buf);
    buf[pos++] = ',';
    pos += printInt(n2, &buf[pos]);
    buf[pos++] = '\n';
    buf[pos++] = '\0';
    return pos - 1;
}

