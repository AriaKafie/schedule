// Aria Kafie 828109926
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <unistd.h>

#include "log.h"
#include "types.h"

void* start_scheduling(void *ptr)
{
    // convert ptr to the appropriate object
    SchedulerInfo *si = (SchedulerInfo *)ptr;

    // error checking variables
    std::ifstream input_file(si->filename);
    std::string input;

    // check for various conditions that would raise an error
    while (std::getline(input_file, input))
    {
        std::istringstream is(input);
        std::string token;
        int count;
        for (count = VALUE_0; is >> token; count++)
        {
            if (std::stof(token) <= FLOAT_0) // bursts must be positive
            {
                std::cout << "A burst number must be bigger than 0" << std::endl;
                si->running = false;
                pthread_exit(ERROR_EXIT);
            }
        }

        if (count % VALUE_2 == VALUE_0) // total bursts must be an odd number
        {
            std::cout << "There must be an odd number of bursts for each process" << std::endl;
            si->running = false;
            pthread_exit(ERROR_EXIT);            
        }
    }

    std::vector<Process*> finished_processes; // arraylist of finished processes
    int global_time = VALUE_0; // track global time
    ProcessQueue ready_q(si->alpha), io_q(si->alpha); // define both queues
    
    std::ifstream burst_file(si->filename); // open the burst file
    if (!burst_file.is_open())
    {
        // burst file must exist otherwise raise an error
        std::cout << "Unable to open <<" << si->filename << ">>" << std::endl;
        si->running = false;
        pthread_exit(ERROR_EXIT);
    }

    // parse each line of the file and push its data to each process
    for (std::string line; std::getline(burst_file, line);)
    {
        ready_q.push(new Process(ready_q.size(), line));
        printf("P%d: %s\n", ready_q.size() - VALUE_1, line.c_str()); // print out a summary of the processes
    }

    ready_q.sort(); // sort the ready queue initially

    do
    {
        // handle there being a process ready for cpu
        if (!ready_q.empty())
        {
            // get the proc's pointer
            Process *cpu_front = ready_q.front();

            // get various data from the process, update global time
            int time_spent = cpu_front->next_burst();
            global_time += time_spent;

            // run the process
            cpu_front->run_cpu(si->alpha);

            // run any parallel io processes concurrently
            io_q.run_io(time_spent);

            while (!io_q.empty())
            {
                // check for finished blocked processes
                Process *front = io_q.front();
                if (front->next_burst() <= VALUE_0)
                {
                    // pop them from the io queue and push them to the ready queue
                    front->bursts.erase(front->bursts.begin());
                    ready_q.push(front);
                    io_q.pop();
                }
                else break;
            }

            // remove the recently run process
            ready_q.pop();

            // if it's not done, push it to blocked queue
            if (!cpu_front->done())
                io_q.push(cpu_front);
            else
            {
                // otherwise calculate turnaround time and add the process to
                // 'finished_processes'
                cpu_front->turnaround_time = global_time;
                finished_processes.push_back(cpu_front);
            }

            // log the cpu burst
            log_cpuburst_execution(cpu_front->id,
                                   cpu_front->cpu_time,
                                   cpu_front->io_time,
                                   global_time,
                                   cpu_front->done() ? COMPLETED : ENTER_IO);
        }
        // handle there being nothing in the ready queue
        else
        {
            // get time to spend before next ready queue push
            int time_spent = io_q.front()->next_burst();
            // add to global time in preparation of running this burst
            global_time += time_spent;

            // run all io proc's at the same time
            io_q.run_io(time_spent);

            while (!io_q.empty())
            {
                // remove any finished io processes
                Process *front = io_q.front();
                if (front->next_burst() <= VALUE_0)
                {
                    front->bursts.erase(front->bursts.begin());
                    // push them to the ready queue
                    ready_q.push(front);
                    io_q.pop();
                }
                else break;
            }
        }

        // sort both queues
        ready_q.sort();
        io_q.sort_io();

        //continue until both queues are empty
    } while(!ready_q.empty() || !io_q.empty());

    // log finished processes and their data
    for (Process *p : finished_processes)
        log_process_completion(p->id,
                               p->turnaround_time,
                               p->turnaround_time - p->cpu_time - p->io_time);

    // log the prediction values of each finished process
    for (Process *p : finished_processes)
    {
        std::istringstream is(p->bursts_s);
        std::string token;

        for (int i = VALUE_0, io = false; is >> token; i++, io = !io)
            if (io)
                p->predictions.insert(p->predictions.begin() + i, std::stof(token));
        
        log_process_estimated_bursts(p->id,
                                     p->predictions.data(),
                                     p->predictions.size());
    }

    // set the shared boolean to false indicating worker is done
    si->running = false;

    // exit
    pthread_exit(NORMAL_EXIT);
}

int main(int argc, char** argv)
{
    // get the option and its corresponding alpha
    char option = getopt(argc, argv, "a:");
    float alpha = option == 'a' ? std::stof(optarg) : NO_VALUE;

    // make sure alpha is valid
    if (option == 'a' && (alpha <= FLOAT_0 || alpha >= FLOAT_1))
    {
        std::cout << "Alpha for exponential averaging must be within (0.0, 1.0)" << std::endl;
        return ERROR_EXIT;
    }

    // get the mandatory path argument
    char *path = optind < argc ? argv[optind] : argv[SECOND_ARG_INDEX];

    // initialize a worker thread
    pthread_t worker;
    // initialize an info object to be passed to the thread
    SchedulerInfo si = { path, alpha, true };

    // instantiate the worker
    pthread_create(&worker, NULL, (void *)&start_scheduling, (void *)&si);

    // busy wait while the worker is running
    while (si.running);
}
