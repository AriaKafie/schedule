
#include "types.h"

#include <algorithm>
#include <sstream>

// helper to run the front process
void Process::run_cpu(float alpha)
{
    // store the prediction
    predictions.push_back(estimate(alpha));
    last_burst = bursts[0];
    cpu_time += bursts[0];
    // remove the process
    bursts.erase(bursts.begin());
}

// Process constructor
Process::Process(int pid, const std::string& _bursts) : id(pid), bursts_s(_bursts), cpu_time(0), io_time(0)
{
    // prepare burst data for parsing
    std::istringstream is(_bursts);
    std::string token;

    // record each burst
    while (is >> token)
        bursts.push_back(std::stoi(token));
}

// helper to estimate a process's burst time
float Process::estimate(float alpha)
{
    // if theres no alpha, return the real burst time
    if (alpha < 0)
        return bursts[0];

    // if n == 1, simply return the average
    else if (predictions.size() == 0)
    {
        float avg = 0;
        
        for (int i = 0, cpu = true; i < bursts.size(); i++, cpu = !cpu)
            if (cpu)
                avg += bursts[i];

        avg /= (float)((bursts.size() - 1) / 2 + 1);

        return avg;
    }
    // else, return a value derived from the exponential averaging formula
    else
    {
        return alpha * (float)last_burst + (1.0f - alpha) * (float)predictions[predictions.size() - 1];
    }
}

// helper to run all io procs concurrently
void ProcessQueue::run_io(int ms)
{
    // gather all procs into a vector
    std::vector<Process*> processes;

    // fill said vector
    for (Process *p; !q.empty();)
    {
        p = q.front();
        processes.push_back(p);
        q.pop();
    }

    // simulate running each proc
    for (Process *p : processes)
    {
        p->io_time += std::min(p->next_burst(), ms);
        p->bursts[0] -= ms;
    }

    // add procs back to the queue
    for (Process *p : processes)
        q.push(p);
}

// helper to sort the io queue
void ProcessQueue::sort_io()
{
    // collect all processes in a vector
    std::vector<Process*> processes;

    // fill the vector
    for (Process *p; !q.empty();)
    {
        p = q.front();
        processes.push_back(p);
        q.pop();
    }

    // stable sort the vector based on actual burst time
    for (int i = 0; i < processes.size(); i++)
    {
        for (int j = 0; j < processes.size() - 1; j++)
        {
            if (processes[j]->next_burst() > processes[j+1]->next_burst())
            {
                Process *temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }
    // add processes back to the queue
    for (Process *p : processes)
        q.push(p);
}

// helper to sort 
void ProcessQueue::sort()
{
    // collect all procs into a vector
    std::vector<Process*> processes;

    // fill said vector
    for (Process *p; !q.empty();)
    {
        p = q.front();
        processes.push_back(p);
        q.pop();
    }

    // stable sort the vector based on estimates
    for (int i = 0; i < processes.size(); i++)
    {
        for (int j = 0; j < processes.size() - 1; j++)
        {
            if (processes[j]->estimate(alpha) > processes[j+1]->estimate(alpha))
            {
                Process *temp = processes[j];
                processes[j] = processes[j+1];
                processes[j+1] = temp;
            }
        }
    }

    // put all the procs back into the queue
    for (Process *p : processes)
        q.push(p);
}

// helpful debug tool to view the contents of a process queue
std::string ProcessQueue::to_string()
{
    std::queue<Process*> copy = q;
    std::stringstream ss;

    for (Process *p; !copy.empty();)
    {
        p = copy.front();

        ss << "P" << p->id << ": { ";
        for (int i : p->bursts)
        {
            ss << i << " ";
        }
        ss << "}\n";
        
        copy.pop();
    }

    return ss.str();
}
