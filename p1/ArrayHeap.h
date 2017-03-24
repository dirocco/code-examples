#pragma once

#include "BucketTimer.h"
#include "Logging.h"

#include <stdlib.h>
#include <string.h>
#include <functional>
#include <limits>

template<typename T>
struct MinHeapCompare
{
    static constexpr auto Infinity = std::numeric_limits<T>::max();
    static bool cmp(const T& t1, const T& t2) { return t1 < t2; };
};

template<typename T>
struct MaxHeapCompare
{
    static constexpr auto Infinity = std::numeric_limits<T>::min();
    static bool cmp(const T& t1, const T& t2) { return t1 > t2; };
};

template<typename T = int64_t, typename Compare = MinHeapCompare<T> >
class ArrayHeap
{
protected:
    T* m_backing = NULL;
    size_t m_backingSize = 0;
    size_t m_size = 0;

    void grow()
    {
        m_backingSize <<= 1;
        m_backing = (T*)realloc((void*)m_backing, m_backingSize * sizeof(T));
        if(unlikely(!m_backing))
            critical_err(errno, "Couldn't resize ArrayHeap backing");
    }

public:
    ArrayHeap(size_t size = 10)
    : m_backingSize(size)
    {
        int err = -1;
        if(err = posix_memalign((void**)&m_backing, 64, m_backingSize * sizeof(T)))
            critical_err(err, "Couldn't allocate ArrayHeap backing\n");
        for(size_t i = 0; i < m_backingSize; i++)
            m_backing[i] = Compare::Infinity;
    }

    ~ArrayHeap()
    {
        free(m_backing);
    }

    void insert(T t)
    {
        m_size++;
        if(m_size >= m_backingSize)
            grow();

        size_t index = m_size;
        while(Compare::cmp(t, m_backing[index / 2]) && index != 0)
        {
            m_backing[index] = m_backing[index / 2];
            index /= 2;
        }
        m_backing[index] = t;
    }

    void pop()
    {
        if(unlikely(m_size == 0))
            err("called pop on empty ArrayHeap\n");
        T tmp = m_backing[m_size--];
        replaceTop(tmp);
    }

    void replaceTop(T t)
    {
        int topIndex = 0;
        int next = 1;
        if(next < m_size && !Compare::cmp(m_backing[next], m_backing[next + 1]))
            next++;

        m_backing[topIndex] = t;

        while(next <= m_size && !Compare::cmp(m_backing[topIndex], m_backing[next]))
        {
            std::swap(m_backing[topIndex], m_backing[next]);
            topIndex = next;
            next *= 2;
            if(next < m_size && !Compare::cmp(m_backing[next] , m_backing[next + 1]))
                next++;
        }
    }

    T getTop()
    {
        if(unlikely(m_size == 0))
            err("called getTop on empty ArrayHeap\n");
        return m_backing[0];
    }

    size_t size()
    {
        return m_size;
    }

    void dbgPrint()
    {
        for(size_t i = 0; i < m_size + 1; i++)
        {
            dbg("%4ld ", m_backing[i]);
        }
        dbg("\n");
    }
};

typedef ArrayHeap<int64_t, MinHeapCompare<int64_t>> MinHeap;
typedef ArrayHeap<int64_t, MaxHeapCompare<int64_t>> MaxHeap;


template<typename T = int64_t, typename Compare = MinHeapCompare<T>>
class KthHeap : public ArrayHeap<T, Compare>
{
    typedef ArrayHeap<T, Compare> Base;
    size_t m_maxSize = 0;

public:
    KthHeap(size_t size = 10)
    :   ArrayHeap<T, Compare>(size),
        m_maxSize(size)
    {}

    void insert(T t)
    {
        if(Base::m_size <= m_maxSize)
            Base::insert(t);
        else if(!Compare::cmp(t, Base::m_backing[0]))
            Base::replaceTop(t);
    }
};

typedef KthHeap<int64_t, MaxHeapCompare<int64_t>> KthSmallest;
typedef KthHeap<int64_t, MinHeapCompare<int64_t>> KthLargest;


template<typename T = int64_t>
class Median
{
    ArrayHeap<T, MinHeapCompare<T>> m_minHeap;
    ArrayHeap<T, MaxHeapCompare<T>> m_maxHeap;

    T m_median = 0;

public:
    Median(size_t size = 4096)
    :   m_minHeap(size),
        m_maxHeap(size)
    {}

    void insert(T t)
    {
        TIMER_START(MedianInsert, 10240);

        if(m_minHeap.size() == m_maxHeap.size())
        {
            if(t > m_median)
            {
                m_minHeap.insert(t);
            }
            else
            {
                m_maxHeap.insert(t);
            }
        }
        else if(m_minHeap.size() > m_maxHeap.size())
        {
            if(t > m_median)
            {
                m_maxHeap.insert(m_minHeap.getTop());
                m_minHeap.replaceTop(t);
            }
            else m_maxHeap.insert(t);
        }
        else if(m_maxHeap.size() > m_minHeap.size())
        {
            if(t < m_median)
            {
                m_minHeap.insert(m_maxHeap.getTop());
                m_maxHeap.replaceTop(t);
            }
            else m_minHeap.insert(t);
        }

        if(m_minHeap.size() > m_maxHeap.size())
            m_median = m_minHeap.getTop();
        else if(m_maxHeap.size() > m_minHeap.size())
            m_median = m_maxHeap.getTop();
        else
            m_median = (m_minHeap.getTop() + m_maxHeap.getTop()) / 2;

        TIMER_STOP(MedianInsert);
    }

    T getMedian()
    {
        return m_median;
    }

    void dbgPrint()
    {
        dbg("minHeap: "); m_minHeap.dbgPrint();
        dbg("maxHeap: "); m_maxHeap.dbgPrint();
    }
};
