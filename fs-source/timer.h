#ifndef HIGHRES_TIMER_H_
#define HIGHRES_TIMER_H_

#define NOMINMAX
#include <Windows.h>

struct High_Res_Timer {
    LARGE_INTEGER start_time;
    double        cpu_frequency;

    High_Res_Timer() {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        cpu_frequency = double(freq.QuadPart);
        QueryPerformanceCounter(&start_time);
    }

    void Reset() {
        QueryPerformanceCounter(&start_time);
    }

    double GetTimeMs() {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return ((double(now.QuadPart - start_time.QuadPart) * 1000.0) / cpu_frequency);
    }

    double GetTimeMicro() {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return ((double(now.QuadPart - start_time.QuadPart) * 1000000.0) / cpu_frequency);
    }

    static High_Res_Timer *it() {
        static High_Res_Timer timer;
        return &timer;
    }

    static double msec() {
        return it()->GetTimeMs();
    }

    static double microsec() {
        return it()->GetTimeMicro();
    }
};

#endif
