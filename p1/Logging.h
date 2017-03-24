#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define dbg(...) fprintf(stderr, __VA_ARGS__)
#define dbg(...)

#ifndef unlikely
#define unlikely(e) __builtin_expect(!!(e), 0)
#endif

#ifndef likely
#define likely(e) __builtin_expect(!!(e), 1)
#endif

#define err(s, ...) fprintf(stderr, "\033[1;31m" s "\033[0m", ##__VA_ARGS__)
#define critical_err(e, s, ...) { fprintf(stderr, s "\t%s\n", ##__VA_ARGS__, strerror(e)); exit(e); }

template<typename T, int bufLen = 128>
class stackPrint
{
public:
    char buf[bufLen] = {0};

    stackPrint(const T& t)
    {
        toString(buf, bufLen, t);
    }
};

#define stackString(s) stackPrint<decltype(s)>(s).buf

