#include "ArrayHeap.h"
#include "Atoi.h"

#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    char* buf;
    size_t len;

    Median<int64_t> median;
    KthSmallest kth(10);

#ifndef USE_STDLIB
    char outbuf[4 * 4096];
    size_t outlen = 0;
    setvbuf(stdout, NULL, _IOFBF, sizeof(outbuf));
#endif

    while(-1 != getline(&buf, &len, stdin))
    {
        if(!len || (!isdigit(buf[0])) && buf[0] != '-')
            continue;

        int64_t n = atoi64(buf);
        median.insert(n);
        kth.insert(n);

#ifndef USE_STDLIB
        if(unlikely(sizeof(outbuf) - outlen < 80))
        {
            if(unlikely(0 > fwrite(outbuf, outlen, 1, stdout)))
                critical_err(errno, "Unable to write to stdout");
            outlen = 0;
        }
        outlen += printPair(outbuf + outlen, median.getMedian(), kth.getTop());
#else
        printf("%ld,%ld\n", median.getMedian(), kth.getTop());
#endif
    }

#ifndef USE_STDLIB
    fwrite(outbuf, outlen, 1, stdout);
#endif
}

