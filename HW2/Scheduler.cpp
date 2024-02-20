#include "Scheduler.h"
using namespace std;

bool comparePriorityRMA(const Instance& I1, const Instance& I2) {
    return I1.D < I2.D;
}

bool comparePriorityEDF(const Instance& I1, const Instance& I2) {
    return I1.absolute_deadline < I2.absolute_deadline;
}

void Scheduler::process_rma_ss() {
        //Insert any new tasks for this clock
        for(int idx = 0; idx < s_tasks.size(); idx++) {

            //For the periodic tasks
            if((s_clock - s_tasks[idx].I)%s_tasks[idx].T == 0) {
                Instance I(s_tasks[idx].C, s_tasks[idx].T, s_tasks[idx].D, s_tasks[idx].I);
                I.taskId = s_tasks[idx].taskId;
                I.instanceId = s_tasks[idx].numInstances;
                s_tasks[idx].numInstances++;

                available_tasks.push_back(I);
                if(debug_p) cout << s_clock << ": Adding Task " << idx << " taskId: " << I.taskId << endl;
            }
        }

        for(int idx = 0; idx < aperiodic_tasks.size(); idx++) {
            //For the aperiodic tasks
            if(aperiodic_tasks[idx].I == s_clock) {
                Instance I(aperiodic_tasks[idx].C, aperiodic_tasks[idx].T, aperiodic_tasks[idx].D, aperiodic_tasks[idx].I);
                I.taskId = aperiodic_tasks[idx].taskId;
                I.instanceId = aperiodic_tasks[idx].numInstances;
                aperiodic_tasks[idx].numInstances++;

                available_aperiodic_tasks.push(I);
                if(debug_ap) cout << s_clock << " : Adding Task " << idx << " taskId: " << I.taskId << endl;
            }
        }

        //Sort the deque
        sort(available_tasks.begin(), available_tasks.end(), comparePriorityRMA);


        //Replenish if it is time
        if(SS.replenishment_times.count(s_clock)) {
            if(idle_idx < replenish_idx) cerr << "How can idle index be less than replenish index " << endl;

            SS.capacity += replenish_capacity[replenish_idx];//.replenishment_times[s_clock]; //TODO: Replenish by the correct amount
            
            if(debug_ap) {
                cout << s_clock << " : Idle idx " << idle_idx << " Replenish idx " << replenish_idx << endl;
                cout << s_clock << " : Replenishing by an amount" << replenish_capacity[replenish_idx] << endl;
                cout << s_clock << " : Server capacity now => " << SS.capacity << endl;
            }

            SS.replenishment_times.erase(s_clock);
            replenish_idx++;
        }

        bool scheduleSS = false;

        //Comparing SS priority with the highest periodic task
        if(!available_aperiodic_tasks.empty() && SS.capacity > 0) {
            //If SS has higher priority
            if(available_tasks.empty()) {
                scheduleSS = true;
            }
            else if(!available_tasks.empty() && SS.D < available_tasks.front().D) {
                scheduleSS = true;
            }

            if(scheduleSS) {
                if(idle_idx >= replenish_capacity.size()) {
                    replenish_capacity.push_back(1);
                    if(debug_ap) cout << s_clock << " : For Idle idx " << idle_idx << " Adding replenish capacity now " << replenish_capacity.size() << endl;
                }
                else {
                    if(debug_ap) cout << s_clock << " : For Idle idx " << idle_idx << " Incrementing replenish capacity " << replenish_capacity.size() << endl;

                    replenish_capacity[idle_idx]++;
                }
            }
        }

        if(!scheduleSS) {
            if(idle_idx >= replenish_capacity.size()) {
                replenish_capacity.push_back(0);
                if(debug_ap) cout << s_clock << " : For Idle idx " << idle_idx << " Adding replenish capacity now " << replenish_capacity.size() << endl;
            }
        }



        //Move to idle if Lower Priority Periodic task is going on
        if(SS.isActive && ((!scheduleSS && !available_tasks.empty() && available_tasks.front().D > SS.D) || (available_tasks.empty() && SS.capacity <= 0) || (available_tasks.empty() && available_aperiodic_tasks.empty()))) {
            if(debug_ap) cout << s_clock << " : Moving to Idle " << s_clock << endl;
            SS.isActive = false;
            idle_idx++;
        }


        //SS is Active if Capacity > 0 & a higher priority task is going on or SS is going on
        if(!SS.isActive && ((scheduleSS && SS.capacity > 0) || (!available_tasks.empty() && available_tasks.front().D < SS.D))) {
            //Moving from Idle to Active
                std::queue<Instance> temp_tasks = available_aperiodic_tasks;
                int rc = 0;
                while(!temp_tasks.empty()) { //FIXME: Check if this logic is correct. Replenishment capacity = Max(SS.T, Sum of aperiodic tasks' C - the processed time of any partially done AP task)
                    rc += temp_tasks.front().C - temp_tasks.front().processed_time;
                    temp_tasks.pop();
                }
                SS.replenishment_times[s_clock + SS.T] = 0; //min(SS.C, rc);)
                //cout << "Setting RT = " << (s_clock + SS.T) << " amount " << SS.replenishment_times[s_clock + SS.T] << endl;

            SS.isActive = true;
        }

        if(scheduleSS && !available_aperiodic_tasks.empty() || (available_tasks.empty() && !available_aperiodic_tasks.empty())) {
            if(SS.capacity > 0) SS.capacity--;
            ss_time++;

            Instance winner_task = available_aperiodic_tasks.front();
            available_aperiodic_tasks.front().processed_time++;
            cout << s_clock << " SS " << winner_task.taskId << endl;
            
            //If done processing then remove
            if(available_aperiodic_tasks.front().processed_time == available_aperiodic_tasks.front().C) {
                SS.num_processed++;
                int response_time = s_clock - available_aperiodic_tasks.front().I + 1; //+1 due to 0 indexed clock
                SS.total_resp_time += response_time;

                /*if(debug_ap)*/ cout << s_clock << " : Removing Aperiodic Task " << available_aperiodic_tasks.front().taskId << " : Response_time: " << response_time << endl;

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
        if(s_clock%SS.T == 0) {
            cout << "Sproradic Serve Time Period = " << SS.T << ": Time Allocated to SS in slot " << s_clock/SS.T << " => " << ss_time << endl;
            ss_time = 0;
        }
}


void Scheduler::setup_engine(int max_clock) {
        for(int idx = 0; idx < max_clock; idx++) clock();
}

void Scheduler::clock() {
    process_rma_ss();
}

