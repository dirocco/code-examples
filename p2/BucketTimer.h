#pragma once

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#ifdef USE_CALLGRIND
#include <valgrind/callgrind.h>
#endif

class BucketTimer
{
    std::vector<uint64_t> m_times;
    size_t m_samples = 0;
    uint64_t m_tic = 0;
    const char* m_name;

public:
    BucketTimer(const char* name, size_t samples)
    :   m_samples(samples),
        m_name(name)
    {
        m_times.reserve(samples);
    }

    void tic()
    {
        uint32_t tsc_aux;
        m_tic = __rdtscp(&tsc_aux);
    }

    void toc()
    {
        uint32_t tsc_aux;
        uint64_t toc = __rdtscp(&tsc_aux);
        m_times.push_back(toc - m_tic);
        if(m_times.size() == m_samples)
        {
            std::sort(m_times.begin(), m_times.end());
            printf("TIMER,%s,%lu,%lu,%lu,%lu,%lu,%lu\n",
                    m_name,
                    m_times[m_times.size() * 0.25], m_times[m_times.size() * 0.5],
                    m_times[m_times.size() * 0.75], m_times[m_times.size() * 0.95],
                    m_times[m_times.size() * 0.99], m_times[m_times.size() * 0.999]);
            m_times.clear();
        }
    }
};

class ScopeTimer
{
    BucketTimer& m_t;
public:
    ScopeTimer(BucketTimer& t)
    : m_t(t)
    {
        m_t.tic();
#ifdef USE_CALLGRIND
        CALLGRIND_START_INSTRUMENTATION;
#endif
    }

    ~ScopeTimer()
    {
#ifdef USE_CALLGRIND
        CALLGRIND_STOP_INSTRUMENTATION;
#endif
        m_t.toc();
    }
};

#ifdef USE_TIMERS

#define TIMER_START_IMPL(name, samples) \
    static BucketTimer s_bucketTimer##name(#name, samples); s_bucketTimer##name.tic();
#define TIMER_STOP_IMPL(name) \
    s_bucketTimer##name.toc();

#define TIMER_SCOPE(name, samples) \
    static BucketTimer s_bucketTimer##name(#name, samples); ScopeTimer scopeTimer##name(s_bucketTimer##name);

#ifdef USE_CALLGRIND
#define TIMER_START(name, samples) TIMER_START_IMPL(name, samples); CALLGRIND_START_INSTRUMENTATION;
#define TIMER_STOP(name) CALLGRIND_STOP_INSTRUMENTATION; TIMER_STOP_IMPL(name);
#else
#define TIMER_START(name, samples) TIMER_START_IMPL(name, samples);
#define TIMER_STOP(name) TIMER_STOP_IMPL(name)
#endif

#else
#define TIMER_START(...)
#define TIMER_STOP(...)
#define TIMER_SCOPE(...)
#endif

