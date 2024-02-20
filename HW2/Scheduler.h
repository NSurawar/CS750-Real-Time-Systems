#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <iostream>
#include <vector>
#include <deque>
#include <set>
#include "/Users/neerajsurawar/git/CS750/HW2/Task.h"

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

    //Reset
    void reset() {
        SS.isActive = false;
        s_clock = 0;
        idle_idx = 0;
        replenish_idx = 0;
        ss_time = 0;

        while(!available_aperiodic_tasks.empty()) available_aperiodic_tasks.pop();

        available_tasks.clear();
        aperiodic_tasks.clear();
        s_tasks.clear();
        replenish_capacity.clear();
    }

    void set_tasks(std::vector<Task>tasks) {
        s_tasks = tasks;
    }
    
    //For SS
    std::vector<int> replenish_capacity;
    int idle_idx = 0;
    int replenish_idx = 0;
    int ss_time = 0;

    bool debug_p = false;
    bool debug_ap  = false;

    Scheduler(std::vector<Task>_tasks) : s_tasks(_tasks) {}
};

#endif