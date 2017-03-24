#include "Atoi.h"
#include "ArrayHeap.h"
#include "BucketTimer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <algorithm>

int main(int argc, char** argv)
{
    srand(time(NULL));

    auto printV = [](const std::vector<int64_t>& v) {
        for(auto& i : v)
            dbg("%4ld ", i);
        dbg("\n");
    };

    {
        printf("\nTesting array heap\n");
        std::vector<int64_t> v;

        MinHeap a(5);
        MaxHeap b(5);

        for(int i = 0; i < 40; i++)
        {
            int64_t n = (rand() % 100) - 50;
            a.insert(n);
            b.insert(n);
            v.push_back(n);
        }

        std::sort(v.begin(), v.end());

        for(size_t i = 0; i < v.size(); i++)
        {
            if(v[i] != a.getTop())
                err("MinHeap mismatch at index %lu got %ld expected %ld\n", i, a.getTop(), v[i]);
            a.pop();
        }

        for(size_t i = v.size() - 1; i > 0; i--)
        {
            if(v[i] != b.getTop())
                err("MaxHeap mismatch at index %lu got %ld expected %ld\n", i, b.getTop(), v[i]);
            b.pop();
        }
    }

    {
        printf("\nTesting Kth Arrayheap\n");
        std::vector<int64_t> v;
        KthSmallest kth(10);
        for(int i = 0; i < 40; i++)
        {
            int64_t n = (rand() % 100) - 50;
            kth.insert(n);
            v.push_back(n);
        }

        std::sort(v.begin(), v.end());
        if(v[10] != kth.getTop())
        {
            err("kth mismatch\n");
            kth.dbgPrint();
            printV(v);
        }
    }

    {
        printf("\nTesting median\n");
        auto testMedian = [&](int size)
        {
            std::vector<int64_t> v;
            Median<int64_t> m(5);
            for(int i = 0; i < size; i++)
            {
                int64_t n = (rand() % 100) - 50;
                m.insert(n);
                v.push_back(n);
            }
            std::sort(v.begin(), v.end());

            uint64_t vMedian = v.size() % 2 == 1 ? v[v.size()/2] : (v[v.size()/2] + v[v.size()/2-1]) / 2;
            if(vMedian != m.getMedian())
            {
                err("Median mismatch for length %d: got %ld, expected: %ld\n", size, m.getMedian(), vMedian);
                m.dbgPrint();
                printV(v);
            }
        };

        testMedian(2);
        testMedian(20);
        testMedian(21);
        testMedian(1);

#ifdef USE_TIMERS
        testMedian(100000);
#endif
    }

    {
        printf("\nTesting atoi\n");
        auto testAtoi = [](int64_t num) {
            char buf[30] = {0};
            snprintf(buf, 30, "%ld", num);
            int64_t result = atoi64(buf);
            if(result != num)
                err("atoi test failure, got %ld expected %ld\n", result, num);
        };

        testAtoi(0);
        testAtoi(1);
        testAtoi(0xffffffff);
        testAtoi(0xffffffffffffffff);
        testAtoi(-1);
        testAtoi(std::numeric_limits<int64_t>::max());
        testAtoi(std::numeric_limits<int64_t>::min());

        for(int i = 0; i < 40; i++)
        {
            int64_t num = rand() - (RAND_MAX / 2);
            testAtoi(num);
        }

        auto expect = [](const char* buf, int64_t expected)
        {
            int64_t n = atoi64(buf);
            if(n != expected)
                err("For buffer '%s', got %ld expected %ld\n", buf, n, expected);
        };
        expect(" ", 0);
        expect("\n", 0);
        expect("asdlkfjhasdf", 0);
        expect("123asdf", 123);
        expect("  -1234", -1234);
        expect("1 ", 1);
        expect("  -1234  ", -1234);
        expect("-1 ", -1);

#ifdef USE_TIMERS
        for(int i = 0; i < 100000; i++)
        {
            TIMER_START(atoi, 1024);
            atoi64("12345");
            TIMER_STOP(atoi);
        }
#endif
    }

    {
        printf("\nTesting printInt\n");
        auto expect = [](int64_t num)
        {
            char snprintfBuf[30] = {0};
            char printIntBuf[30] = {0};
            size_t ss = snprintf(snprintfBuf, 30, "%ld", num);
            size_t pis = printInt(num, printIntBuf);
            if(ss != pis)
                err("printInt size: %lu, snprintf size: %lu for %ld\n", pis, ss, num);
            if(memcmp(printIntBuf, snprintfBuf, std::max(ss, pis)))
                err("printInt failure, expected %s got %s\n", snprintfBuf, printIntBuf);
        };

        expect(std::numeric_limits<int64_t>::min());
        expect(std::numeric_limits<int64_t>::min() + 1);
        expect(std::numeric_limits<int64_t>::max());
        expect(12345);
        expect(123456);
        expect(-12345);
        expect(-123456);
        expect(10);
        expect(0);
        expect(-1);
        expect(1);

        auto pair = [](int64_t n1, int64_t n2)
        {
            char snprintfBuf[50] = {0};
            char printIntBuf[50] = {0};
            size_t ss = snprintf(snprintfBuf, 50, "%ld,%ld\n", n1, n2);
            size_t pis = printPair(printIntBuf, n1, n2);
            if(ss != pis)
                err("printInt size: %lu, snprintf size: %lu for %ld,%ld\n", pis, ss, n1, n2);
            if(memcmp(printIntBuf, snprintfBuf, std::max(ss, pis)))
                err("printInt failure, expected %s got %s\n", snprintfBuf, printIntBuf);
        };

        pair(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min());
        pair(std::numeric_limits<int64_t>::min() + 1, std::numeric_limits<int64_t>::min() + 1);
        pair(std::numeric_limits<int64_t>::max() + 1, std::numeric_limits<int64_t>::max() + 1);
        pair(12345, 12345);
        pair(-12345, -12345);
        pair(123456, 123456);
        pair(-123456, -123456);
        pair(10, -10);
        pair(0, 0);
        pair(-1, 0);
        pair(1, -1);
    }
}
