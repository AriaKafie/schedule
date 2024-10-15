
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <unistd.h>

#include "log.h"
#include "types.h"

constexpr int NORMAL_EXIT = 0;
constexpr int ERROR_EXIT = 1;

void* start_scheduling(void *ptr)
{
    SchedulerInfo *si = (SchedulerInfo *)ptr;

    // error checking
    std::ifstream input_file(si->filename);
    std::string input;

    while (std::getline(input_file, input))
    {
        std::istringstream is(input);
        std::string token;
        int count;
        for (count = 0; is >> token; count++)
        {
            if (std::stof(token) <= 0.0f)
            {
                std::cout << "A burst number must be bigger than 0" << std::endl;
                si->running = false;
                pthread_exit(ERROR_EXIT);
            }
        }

        if (count % 2 == 0)
        {
            std::cout << "There must be an odd number of bursts for each process" << std::endl;
            si->running = false;
            pthread_exit(ERROR_EXIT);            
        }
    }

    std::vector<Process*> finished_processes;
    int global_time = 0;
    ProcessQueue ready_q(si->alpha), io_q(si->alpha);
    
    std::ifstream burst_file(si->filename);
    if (!burst_file.is_open())
    {
        std::cout << "Unable to open <<" << si->filename << ">>" << std::endl;
        si->running = false;
        pthread_exit(0);
    }
    
    for (std::string line; std::getline(burst_file, line);)
    {
        ready_q.push(new Process(ready_q.size(), line));
        printf("P%d: %s\n", ready_q.size() - 1, line.c_str());
    }

    ready_q.sort();

    do
    {
        if (!ready_q.empty())
        {
            Process *cpu_front = ready_q.front();

            int time_spent = cpu_front->next_burst();
            global_time += time_spent;
            cpu_front->run_cpu(si->alpha);

            io_q.run_io(time_spent);

            while (!io_q.empty())
            {
                Process *front = io_q.front();
                if (front->next_burst() <= 0)
                {
                    front->bursts.erase(front->bursts.begin());
                    ready_q.push(front);
                    io_q.pop();
                }
                else break;
            }

            ready_q.pop();
            
            if (!cpu_front->done())
                io_q.push(cpu_front);
            else
            {
                cpu_front->turnaround_time = global_time;
                finished_processes.push_back(cpu_front);
            }

            log_cpuburst_execution(cpu_front->id,
                                   cpu_front->cpu_time,
                                   cpu_front->io_time,
                                   global_time,
                                   cpu_front->done() ? COMPLETED : ENTER_IO);

            // printf("P%d: cpu executed = %d, io executed = %d, time elapsed = %d, %s\n",
            //        cpu_front->id, cpu_front->cpu_time, cpu_front->io_time, global_time, cpu_front->done() ? "completed" : "enter io");
        }
        else
        {
            int time_spent = io_q.front()->next_burst();
            global_time += time_spent;

            io_q.run_io(time_spent);

            while (!io_q.empty())
            {
                Process *front = io_q.front();
                if (front->next_burst() <= 0)
                {
                    front->bursts.erase(front->bursts.begin());
                    ready_q.push(front);
                    io_q.pop();
                }
                else break;
            }
        }

        ready_q.sort();
        io_q.sort_io();
        
    } while(!ready_q.empty() || !io_q.empty());

    for (Process *p : finished_processes)
        log_process_completion(p->id,
                               p->turnaround_time,
                               p->turnaround_time - p->cpu_time - p->io_time);

    for (Process *p : finished_processes)
    {
        std::istringstream is(p->bursts_s);
        std::string token;

        for (int i = 0, io = false; is >> token; i++, io = !io)
        {
            if (io)
            {
                p->predictions.insert(p->predictions.begin() + i, std::stof(token));
                //i++;
            }
        }
        
        log_process_estimated_bursts(p->id,
                                     p->predictions.data(),
                                     p->predictions.size());
    }
    
    // for (Process *p : finished_processes)
    //     std::cout << p->bursts_s << std::endl;
    
    si->running = false;
    pthread_exit(NORMAL_EXIT);
}

int main(int argc, char** argv)
{
    float alpha = getopt(argc, argv, "a:") == 'a' ? std::stod(optarg) : NO_VALUE;

    if (getopt(argc, argv, "a:") == 'a' && (alpha <= 0.0f || alpha >= 1.0f))
    {
        std::cout << "Alpha for exponential averaging must be within (0.0, 1.0)" << std::endl;
        return ERROR_EXIT;
    }
    
    char *path = optind < argc ? argv[optind] : argv[SECOND_ARG_INDEX];

    pthread_t worker;
    SchedulerInfo si = { path, alpha, true };

    pthread_create(&worker, NULL, (void *)&start_scheduling, (void *)&si);

    while (si.running);
}
