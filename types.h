
#ifndef TYPES_H
#define TYPES_H

constexpr double NO_ALPHA         = -1;
constexpr int    SECOND_ARG_INDEX =  1;

struct SchedulerInfo {
    char *filename;
    double alpha;
    bool running;
};

#endif
