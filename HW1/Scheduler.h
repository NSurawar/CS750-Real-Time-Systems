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
    //multiset<Instance> available_tasks;
    std::deque<Instance> available_tasks;

    int s_clock = 0;    //Clock counter;

    void clock_rma();

    void process_rma(int max_clock);

    Scheduler(std::vector<Task>_tasks) : s_tasks(_tasks) {}
};

#endif