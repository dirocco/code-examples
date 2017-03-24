#pragma once

#ifdef USE_TIMERS
#define EngineTM TMNull
#include "TMNull.h"
#else
#define EngineTM TMBookPrint
#include "TMBookPrint.h"
#endif

