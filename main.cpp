
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
    std::string line;
    
    for (int pid = 0; std::getline(burst_file, line); pid++)
        ready_q.push(new Process(pid, line));

    std::cout << ready_q.to_string() << std::endl;

    ready_q.sort();

    std::cout << ready_q.to_string() << std::endl;

    do
    {
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
