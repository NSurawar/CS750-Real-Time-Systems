#include "Scheduler.h"
using namespace std;

bool comparePriorityRMA(const Instance& I1, const Instance& I2) {
    return I1.D < I2.D;
}

bool comparePriorityEDF(const Instance& I1, const Instance& I2) {
    return I1.absolute_deadline < I2.absolute_deadline;
}

void Scheduler::process_rma() {
        //Insert any new tasks for this clock
        for(int idx = 0; idx < s_tasks.size(); idx++) {
            if((s_clock - s_tasks[idx].I)%s_tasks[idx].T == 0) {
                Instance I(s_tasks[idx].C, s_tasks[idx].T, s_tasks[idx].D, s_tasks[idx].I);
                I.taskId = s_tasks[idx].taskId;
                I.instanceId = s_tasks[idx].numInstances;
                s_tasks[idx].numInstances++;

                //available_tasks.insert(I);
                available_tasks.push_back(I);
                cout << s_clock << ": Adding Task " << idx << " taskId: " << I.taskId << endl;
            }
        }

        //Sort the deque
        sort(available_tasks.begin(), available_tasks.end(), comparePriorityRMA);

        //Process tasks for this clock
        if(!available_tasks.empty()) {
            Instance winner_task = available_tasks.front();
            available_tasks.front().processed_time++;
            cout << s_clock << " " << winner_task.taskId << " " << winner_task.instanceId << endl;

            //If done processing then remove
            if(available_tasks.front().processed_time == available_tasks.front().C) {
                cout << s_clock << ":Removing Task " << available_tasks.front().taskId << endl;
                available_tasks.pop_front();
            }
        }
        //TODO: Check that no 2 instances have the same taskId i.e. no 2 instances of a task are present at a given moment
        s_clock++;
}

void Scheduler::process_edf() {
        //Insert any new tasks for this clock
        for(int idx = 0; idx < s_tasks.size(); idx++) {
            if((s_clock - s_tasks[idx].I)%s_tasks[idx].T == 0) {
                Instance I(s_tasks[idx].C, s_tasks[idx].T, s_tasks[idx].D, s_tasks[idx].I);
                I.taskId = s_tasks[idx].taskId;
                I.instanceId = s_tasks[idx].numInstances;
                I.absolute_deadline = s_clock + I.D;
                s_tasks[idx].numInstances++;

                //available_tasks.insert(I);
                available_tasks.push_back(I);
                cout << s_clock << ": Adding Task " << idx << " taskId: " << I.taskId << endl;
            }
        }

        //Sort the deque
        sort(available_tasks.begin(), available_tasks.end(), comparePriorityEDF);

        //Process tasks for this clock
        if(!available_tasks.empty()) {
            Instance winner_task = available_tasks.front();
            available_tasks.front().processed_time++;
            cout << s_clock << " " << winner_task.taskId << " " << winner_task.instanceId << endl;

            //If done processing then remove
            if(available_tasks.front().processed_time == available_tasks.front().C) {
                cout << s_clock << ":Removing Task " << available_tasks.front().taskId << endl;
                available_tasks.pop_front();
            }
        }
        //TODO: Check that no 2 instances have the same taskId i.e. no 2 instances of a task are present at a given moment
        s_clock++;
}

void Scheduler::setup_engine(int max_clock) {
        for(int idx = 0; idx < max_clock; idx++) clock();
}

void Scheduler::clock() {
    if(schedule_rma) process_rma();
    else process_edf();
}

