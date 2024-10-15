
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
    Process(int pid, const std::string& _bursts);

    bool done() const { return bursts.size() == 0; }
    int next_burst() const { return bursts[0]; }
    float estimate(float ALPHA);
    void run_cpu(float alpha);

    int last_burst;
    int id;
    int cpu_time;
    int io_time;
    int turnaround_time;

    std::string bursts_s;
    std::vector<float> predictions;
    std::vector<int> bursts;
};

class ProcessQueue
{
public:
    ProcessQueue(float _alpha) : alpha(_alpha) {}

    Process* front() const { return q.front(); }
    bool empty() const { return q.size() == 0; }
    int size() const { return q.size(); }
    void push(Process *proc) { q.push(proc); }
    void pop() { q.pop(); }
    void sort();
    void run_io(int ms);
    void sort_io();

    float alpha;
    
    std::string to_string();
    std::queue<Process*> q;
};

#endif
