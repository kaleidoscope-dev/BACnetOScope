
#ifndef TIMER_H
#define TIMER_H

#ifdef BACAPI_BUILD_WIN32
//#include <stdafx.h>
#include <windows.h>
#include <MMSystem.h>
#endif

#include "bacnet.h"
#include <time.h>


//#include <stdbool.h>
#include <stdint.h>

//#include <windows.h>
//#include <winsock.h>
#include <time.h>
//#endif
//
#ifndef MAX_MILLISECOND_TIMERS
#define TIMER_SILENCE 0
#define MAX_MILLISECOND_TIMERS 2
#endif


#define INT8_MIN (-128)
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)

#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647

#define UINT8_MAX 0xff
#define UINT16_MAX 0xffff
#define UINT32_MAX 0xffffffff

//// PORT_COMMENT_START

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};
#endif
// PORT_COMMENT_END


class I_cTimer
{
 public:
        I_cTimer();
       ~I_cTimer();


public:
#if defined BACAPI_BUILD_ARM9 || defined BACAPI_BUILD_LINUX
    uint32_t timeGetTime(void);
#endif
    void timer_init(void);
    int gettimeofday(struct timeval *tp,void *tzp);
    uint32_t timer_milliseconds(unsigned index);
    uint32_t timer_milliseconds();
    bool timer_elapsed_milliseconds(unsigned index,uint32_t value);
    bool timer_elapsed_seconds(unsigned index,uint32_t value);
    bool timer_elapsed_minutes(unsigned index,uint32_t seconds);
    uint32_t timer_milliseconds_set(unsigned index,uint32_t value);

    uint32_t timer_reset(unsigned index);



void StartCounter();

double GetCounter();

};

extern I_cTimer gTimer;

#endif
