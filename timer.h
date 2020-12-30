#pragma once
#include <time.h>


class Timer {
public:
    clock_t f_tp;
    clock_t tp;

    Timer() {
        tp = clock();
        f_tp = tp;
    }

    double time() {
        clock_t cur = clock();
        return (double)(cur - tp) / CLOCKS_PER_SEC;
    }

    double timeAndReset() {
        clock_t cur = clock();
        auto tmp = tp;
        tp = cur;
        return (double)(cur - tmp) / CLOCKS_PER_SEC;
    }

    double t_time() {
        clock_t cur = clock();
        return (double)(cur - f_tp) / CLOCKS_PER_SEC;
    }

    void reset() {
        tp = clock();
    }

};