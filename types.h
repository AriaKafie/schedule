
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <queue>
#include <vector>

// define some constants
#define FLOAT_0 0.0f
#define FLOAT_1 1.0f

constexpr int VALUE_1 = 1;
constexpr int VALUE_0 = 0;
constexpr int VALUE_2 = 2;
constexpr int NORMAL_EXIT = 0;
constexpr int ERROR_EXIT = 1;

constexpr float NO_VALUE         = -1;
constexpr int   SECOND_ARG_INDEX =  1;

// define an info object to be passed to a thread
typedef struct {
    char *filename;
    float alpha;
    bool running;
} SchedulerInfo;

// process class will fill up the queues
class Process
{
public:
    // constructor
    Process(int pid, const std::string& _bursts);

    // helpers
    bool done() const { return bursts.size() == VALUE_0; }
    int next_burst() const { return bursts[VALUE_0]; }
    float estimate(float ALPHA);
    void run_cpu(float alpha);

    // member data
    int last_burst;
    int id;
    int cpu_time;
    int io_time;
    int turnaround_time;

    // member data structures
    std::string bursts_s;
    std::vector<float> predictions;
    std::vector<int> bursts;
};

// ProcessQueue, a wrapper for std::queue<Process*>
class ProcessQueue
{
public:
    // constructor
    ProcessQueue(float _alpha) : alpha(_alpha) {}

    // helper functions
    Process* front() const { return q.front(); }
    bool empty() const { return q.size() == VALUE_0; }
    int size() const { return q.size(); }
    void push(Process *proc) { q.push(proc); }
    void pop() { q.pop(); }
    void sort();
    void run_io(int ms);
    void sort_io();

    // data member
    float alpha;

    // member data structures
    std::string to_string();
    std::queue<Process*> q;
};

#endif
