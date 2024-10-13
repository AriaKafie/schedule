
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "types.h"

void* start_scheduling(void *ptr)
{
    SchedulerInfo *si = (SchedulerInfo *)ptr;

    int num_processes = 0;
    std::string s;
    for (std::ifstream i(si->filename); std::getline(i, s); num_processes++);

    Process *processes = new Process[num_processes];
    
    std::ifstream burst_file(si->filename);
    float alpha = si->alpha;
    ProcessQueue ready_q, io_q;

    std::string line;
    for (int pid = 0; std::getline(burst_file, line); pid++)
    {
        processes[pid] = Process(pid, line);
        ready_q.push(&processes[pid]);
    }

    std::cout << ready_q.to_string();
    
    do
    {
    } while(false);

    delete[] processes;
    
    si->running = false;
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    float alpha = getopt(argc, argv, "a:") == 'a' ? std::stod(optarg) : NO_ALPHA;
    char *path = optind < argc ? argv[optind] : argv[SECOND_ARG_INDEX];

    pthread_t worker;
    SchedulerInfo si = { path, alpha, true };

    pthread_create(&worker, NULL, (void *)&start_scheduling, (void *)&si);

    while (si.running);
}
