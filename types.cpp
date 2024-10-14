
#include "types.h"

#include <sstream>

void Process::run_cpu()
{
    cpu_time += bursts[0];
    bursts.erase(bursts.begin());
}

Process::Process(int pid, const std::string& _bursts) : id(pid), cpu_time(0), io_time(0)
{
    std::istringstream is(_bursts);
    std::string token;

    while (is >> token)
        bursts.push_back(std::stoi(token));
}

float Process::estimate(float alpha)
{
    if (alpha < 0)
        return bursts[0];
    
    else if (estimates.size() == 0)
    {
        float avg = 0;
        
        for (int i = 0, cpu = true; i < bursts.size(); i++, cpu = !cpu)
            if (cpu)
                avg += bursts[i];

        avg /= (float)((bursts.size() - 1) / 2 + 1);

        return avg;
    }
}

void ProcessQueue::run_io(int ms)
{
    std::vector<Process*> processes;

    for (Process *p; !q.empty();)
    {
        p = q.front();
        processes.push_back(p);
        q.pop();
    }

    for (Process *p : processes)
    {
        p->io_time += std::max(p->next_burst(), ms);
        p->bursts[0] -= ms;
    }
}

void ProcessQueue::sort_io()
{
    std::vector<Process*> processes;

    for (Process *p; !q.empty();)
    {
        p = q.front();
        processes.push_back(p);
        q.pop();
    }

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

    for (Process *p : processes)
        q.push(p);
}

void ProcessQueue::sort()
{
    std::vector<Process*> processes;

    for (Process *p; !q.empty();)
    {
        p = q.front();
        processes.push_back(p);
        q.pop();
    }

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

    for (Process *p : processes)
        q.push(p);
}

std::string ProcessQueue::to_string()
{
    std::queue<Process*> copy = q;
    std::stringstream ss;

    for (Process *p; !copy.empty();)
    {
        p = copy.front();

        ss << "P" << p->id << ": { ";
        for (int i = 0; i < p->bursts.size(); i++)
        {
            if (p->bursts[i] != NO_VALUE)
                ss << p->bursts[i] << " ";
        }
        ss << "}\n";
        
        copy.pop();
    }

    return ss.str();
}
