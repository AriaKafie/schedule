
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <queue>
#include <vector>

constexpr float NO_VALUE         = -1;
constexpr int   SECOND_ARG_INDEX =  1;

typedef struct {
    char *filename;
    float alpha;
    bool running;
} SchedulerInfo;

class Process
{
public:
    Process() = default;
    Process(int pid, const std::string& bursts_s);
    
    float estimate(float ALPHA);

    int id;
    int cpu_time;
    int io_time;

    std::vector<int> bursts;
    std::vector<float> estimates;
};

class ProcessQueue
{
public:
    ProcessQueue(float _alpha) : alpha(_alpha) {}
    
    void     push(Process *proc);
    //Process* pop();
    void     sort();

    float alpha;
    std::string to_string();
    std::queue<Process*> q;
};

#endif
