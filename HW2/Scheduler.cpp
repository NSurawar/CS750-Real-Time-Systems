#include "Scheduler.h"
using namespace std;

bool comparePriorityRMA(const Instance& I1, const Instance& I2) {
    return I1.D < I2.D;
}

bool comparePriorityEDF(const Instance& I1, const Instance& I2) {
    return I1.absolute_deadline < I2.absolute_deadline;
}

//RMA with Sporadic Server
//TODO: Add aperiodic tasks vector
//When Aperiodic task is available - Check sporadic server priority with all the highest (first) priority task in the periodic tasks queue
//Add Active and capacity field for Sporadic Server
//Keep a queue for replenishment times - How to keep track of the consumed capacity?

void Scheduler::process_rma_ss() {
        //Insert any new tasks for this clock
        for(int idx = 0; idx < s_tasks.size(); idx++) {

            //For the periodic tasks
            if((s_clock - s_tasks[idx].I)%s_tasks[idx].T == 0) {
                Instance I(s_tasks[idx].C, s_tasks[idx].T, s_tasks[idx].D, s_tasks[idx].I);
                I.taskId = s_tasks[idx].taskId;
                I.instanceId = s_tasks[idx].numInstances;
                s_tasks[idx].numInstances++;

                //available_tasks.insert(I);
                available_tasks.push_back(I);
                //cout << s_clock << ": Adding Task " << idx << " taskId: " << I.taskId << endl;
            }
        }

        for(int idx = 0; idx < aperiodic_tasks.size(); idx++) {
            //For the aperiodic tasks
            if(aperiodic_tasks[idx].I == s_clock) {
                Instance I(aperiodic_tasks[idx].C, aperiodic_tasks[idx].T, aperiodic_tasks[idx].D, aperiodic_tasks[idx].I);
                I.taskId = aperiodic_tasks[idx].taskId;
                I.instanceId = aperiodic_tasks[idx].numInstances;
                aperiodic_tasks[idx].numInstances++;

                //available_tasks.insert(I);
                available_aperiodic_tasks.push(I);
                //aperiodic_tasks.pop(); //Remove the Aperiodic task once done - TODO: Can't be done with vector but queue only works if the aperiodic tasks are pushed in order
                //cout << s_clock << ": Adding Task " << idx << " taskId: " << I.taskId << endl;
            }
        }

        //Sort the deque
        sort(available_tasks.begin(), available_tasks.end(), comparePriorityRMA);


        //Replenish if it is time
        if(SS.replenishment_times.count(s_clock)) {
            SS.capacity = SS.replenishment_times[s_clock]; //TODO: Replenish by the correct amount
            SS.replenishment_times.erase(s_clock);
            cout << "Replenishing to " << SS.capacity << endl;
        }
        cout << "SS Capacity = " << SS.capacity << endl;

        //Comparing SS priority with the highest periodic task
        bool scheduleSS = false;

        if(!available_aperiodic_tasks.empty() && SS.capacity > 0) {
            //If SS has higher priority
            if(available_tasks.empty()) scheduleSS = true;
            else if(!available_tasks.empty() && SS.D < available_tasks.front().D) {
                scheduleSS = true;
            }
            //SS.isActive = true;
            //SS.replenishment_times[s_clock + SS.T] = 0;//.push(s_clock + SS.T);
        }//else if(available_tasks.empty() && !available_aperiodic_tasks.empty()) scheduleSS = true;

        //Move to idle if Lower Priority Periodic task is going on
        if(SS.isActive && ((!scheduleSS && !available_tasks.empty() && available_tasks.front().D > SS.D) || (available_tasks.empty() && SS.capacity <= 0) || (available_tasks.empty() && available_aperiodic_tasks.empty()))) {
            cout << "Moving to Idle " << s_clock << endl;
            SS.isActive = false;
        }


        //SS is Active if Capacity > 0 & a higher priority task is going on 
        //or SS is going on
        if(!SS.isActive && ((scheduleSS && SS.capacity > 0) || (!available_tasks.empty() && available_tasks.front().D < SS.D))) {
            //Moving from Idle to Active
                std::queue<Instance> temp_tasks = available_aperiodic_tasks;
                int rc = 0;
                while(!temp_tasks.empty()) { //FIXME: Check if this logic is correct. Replenishment capacity = Max(SS.T, Sum of aperiodic tasks' C - the processed time of any partially done AP task)
                    rc += available_aperiodic_tasks.front().C - available_aperiodic_tasks.front().processed_time;
                    temp_tasks.pop();
                }
                cout << "Setting RT = " << (s_clock + SS.T) << endl;
                SS.replenishment_times[s_clock + SS.T] = max(SS.C, rc);

            SS.isActive = true;
        }

        if(scheduleSS && !available_aperiodic_tasks.empty() || (available_tasks.empty() && !available_aperiodic_tasks.empty())) {
            if(SS.capacity > 0) SS.capacity--;
            Instance winner_task = available_aperiodic_tasks.front();
            available_aperiodic_tasks.front().processed_time++;
            cout << s_clock << " SS " << winner_task.taskId << endl;
            
            //If done processing then remove
            if(available_aperiodic_tasks.front().processed_time == available_aperiodic_tasks.front().C) {
                //cout << s_clock << ":Removing Task " << available_tasks.front().taskId << endl;
                available_aperiodic_tasks.pop();
            }
        }
        //Process tasks for this clock
        else if(!available_tasks.empty() && scheduleSS == false) {
            Instance winner_task = available_tasks.front();
            available_tasks.front().processed_time++;
            cout << s_clock << " " << winner_task.taskId << " " << winner_task.instanceId << endl;

            //If done processing then remove
            if(available_tasks.front().processed_time == available_tasks.front().C) {
                //cout << s_clock << ":Removing Task " << available_tasks.front().taskId << endl;
                available_tasks.pop_front();
            }
        }
        else cout << s_clock << " -1 -1" << endl;
        
        //TODO: 
        //For HWs where T < D - Check that no 2 instances have the same taskId i.e. no 2 instances of a task are present at a given moment
        s_clock++;
}

void Scheduler::process_rma() {
        //Insert any new tasks for this clock
        for(int idx = 0; idx < s_tasks.size(); idx++) {

            //For the periodic tasks
            if((s_clock - s_tasks[idx].I)%s_tasks[idx].T == 0) {
                Instance I(s_tasks[idx].C, s_tasks[idx].T, s_tasks[idx].D, s_tasks[idx].I);
                I.taskId = s_tasks[idx].taskId;
                I.instanceId = s_tasks[idx].numInstances;
                s_tasks[idx].numInstances++;

                //available_tasks.insert(I);
                available_tasks.push_back(I);
                //cout << s_clock << ": Adding Task " << idx << " taskId: " << I.taskId << endl;
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
                //cout << s_clock << ":Removing Task " << available_tasks.front().taskId << endl;
                available_tasks.pop_front();
            }
        }
        else cout << s_clock << " -1 -1" << endl;
        
        //TODO: 
        //For HWs where T < D - Check that no 2 instances have the same taskId i.e. no 2 instances of a task are present at a given moment
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
                //cout << s_clock << ": Adding Task " << idx << " taskId: " << I.taskId << endl;
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
                //cout << s_clock << ":Removing Task " << available_tasks.front().taskId << endl;
                available_tasks.pop_front();
            }
        }
        else cout << s_clock << " -1 -1" << endl;
        //TODO: 
        //For HWs where T < D - Check that no 2 instances have the same taskId i.e. no 2 instances of a task are present at a given moment
        s_clock++;
}

void Scheduler::setup_engine(int max_clock) {
        for(int idx = 0; idx < max_clock; idx++) clock();
}

void Scheduler::clock() {
    if(schedule_rma) process_rma_ss();
    else process_edf();
}

