
#include "types.h"

#include <sstream>

Process::Process(int pid, const std::string& bursts_s) : id(pid), cpu_time(0), io_time(0)
{
    std::istringstream is(bursts_s);
    std::string token;

    while (is >> token)
        bursts.push_back(std::stoi(token));
}

void ProcessQueue::push(Process *proc)
{
    q.push(proc);
}

std::string ProcessQueue::to_string()
{
    std::queue copy = q;
    std::stringstream ss;

    for (Process *p; !copy.empty();)
    {
        p = copy.front();

        ss << "P" << p->id << ": { ";
        for (int i : p->bursts)
            ss << i << " ";
        ss << "}\n";
        
        copy.pop();
    }

    return ss.str();
}
