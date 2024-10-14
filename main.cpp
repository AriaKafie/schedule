
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "types.h"

void* start_scheduling(void *ptr)
{
    SchedulerInfo *si = (SchedulerInfo *)ptr;
    
    ProcessQueue ready_q(si->alpha), io_q(si->alpha);
    std::ifstream burst_file(si->filename);
    
    for (std::string line; std::getline(burst_file, line);)
        ready_q.push(new Process(ready_q.size(), line));

    ready_q.sort();

    do
    {
        if (!ready_q.empty())
        {
            Process *cpu_front = ready_q.front();

            int time_spent = cpu_front->next_burst();
            cpu_front->run_cpu();

            io_q.run_io(time_spent);
        }
        
    } while(false);

    si->running = false;
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    float alpha = getopt(argc, argv, "a:") == 'a' ? std::stod(optarg) : NO_VALUE;
    char *path = optind < argc ? argv[optind] : argv[SECOND_ARG_INDEX];

    pthread_t worker;
    SchedulerInfo si = { path, alpha, true };

    pthread_create(&worker, NULL, (void *)&start_scheduling, (void *)&si);

    while (si.running);
}
