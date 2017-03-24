#pragma once

enum class Stat
{
#define stat(name, enabled) name,
#include "Stats.h"
#undef stat
    LAST,
};

const char* toString(Stat s);
void incrementStat(Stat s);
void printStatsSummary();

