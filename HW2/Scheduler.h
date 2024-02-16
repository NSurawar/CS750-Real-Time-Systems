#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <iostream>
#include <vector>
#include <deque>
#include <set>
#include "/Users/neerajsurawar/git/CS750/HW1/Task.h"

class Scheduler {
public:
    std::vector<Task> s_tasks;   //Store all tasks
    std::vector<Task> aperiodic_tasks;   //Store all Aperiodic tasks

    //multiset<Instance> available_tasks;
    std::deque<Instance> available_tasks;
    std::queue<Instance> available_aperiodic_tasks;

    SporadicServer SS;

    int s_clock = 0;    //Clock counter;
    bool schedule_rma = true;

    void setup_engine(int max_clock);
    void clock();

    //RMA
    void process_rma();

    //EDF
    void process_edf();

    //RMA with SS
    void process_rma_ss();
    
    Scheduler(std::vector<Task>_tasks) : s_tasks(_tasks) {}
};

#endif