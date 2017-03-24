#include "StatsCounter.h"

#include <stdint.h>
#include <stdio.h>

static uint32_t StatsArray[(int)Stat::LAST] = {0};
static bool StatsArrayEnabled[(int)Stat::LAST] = {
#define stat(name, enabled) enabled,
#include "Stats.h"
#undef stat
};

void incrementStat(Stat s)
{
#ifdef USE_COUNTERS
    if(StatsArrayEnabled[(int)s])
        StatsArray[(int)s]++;
#endif
}

const char* toString(Stat s)
{
    switch(s)
    {
#define stat(name, enabled) case Stat:: name : return #name;
#include "Stats.h"
#undef stat
    };
}

void printStatsSummary()
{
#ifdef USE_COUNTERS
    printf("Counters Summary:\n");
    for(int i = 0; i < (int)Stat::LAST; i++)
    {
        if(StatsArrayEnabled[i])
            printf("\t%s:\t\t%u\n", toString((Stat)i), StatsArray[i]);
    }
    printf("\n");
#endif
}

