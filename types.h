
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <queue>
#include <vector>

constexpr float NO_ALPHA         = -1;
constexpr int   SECOND_ARG_INDEX =  1;

struct SchedulerInfo {
    char *filename;
    float alpha;
    bool running;
};

class Process
{
public:
    Process(int pid, const std::string& bursts);
    
    int id;
    int cpu_time;
    int io_time;

    std::vector<int> bursts;
};

class ProcessQueue
{
public:
    void     push(Process *proc);
    //Process* pop();
    //void     sort();

    std::string to_string();
    
    std::queue<Process*> q;
};

#endif
