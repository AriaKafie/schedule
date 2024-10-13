
#include <iostream>
#include <pthread.h>
#include <unistd.h>

#include "types.h"

void* start_scheduling(void *ptr)
{
    SchedulerInfo *si = (SchedulerInfo *)ptr;

    std::ifstream burst_file(si->filename);
    double alpha = si->alpha;
    
    do
    {
    } while(ready_q not empty || io_q not empty);
    
    si->running = false;
    pthread_exit(0);
}

int main(int argc, char** argv)
{
    double alpha = getopt(argc, argv, "a:") == 'a' ? std::stod(optarg) : NO_ALPHA;
    char *path = optind < argc ? argv[optind] : argv[SECOND_ARG_INDEX];

    pthread_t worker;
    SchedulerInfo si = { path, alpha, true };

    pthread_create(&worker, NULL, (void *)&start_scheduling, (void *)&si);

    while (si.running);
}
