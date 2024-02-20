#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <set>
using namespace std;
#include "/Users/neerajsurawar/git/CS750/HW2/Task.h"
#include "/Users/neerajsurawar/git/CS750/HW2/Scheduler.h"

bool comparePriorityRMA(const Task& I1, const Task& I2) {
    return I1.D < I2.D;
}

//Ignore Phase since worst case response Tme is when all tasks are scheduled at the same Tme.
double calculate_response_Time(int task_index, const vector<Task>& tasks) {
    double response_Time = tasks[task_index].C;

    //Assuming tasks are sorted based on their relative deadlines - Only check for indexes lower than yours
    while(1) {
        double prev_response_Time = response_Time;
        response_Time = tasks[task_index].C;

        for (int j = 0; j < task_index; ++j) {
            const Task& task_j = tasks[j];
            response_Time += (ceil((prev_response_Time) / task_j.T)) * task_j.C;
        }
        //cout << prev_response_Time << " : " << response_Time << endl;
        if(response_Time == prev_response_Time || response_Time > tasks[task_index].D) break;
    }

    return response_Time;
}

bool is_schedulable_rma(const vector<Task>& tasks) {
    for (size_t i = 0; i < tasks.size(); ++i) {
        double response_Time = calculate_response_Time(i, tasks);
        std::cout << "Worst-Case Response time for Task " << i << " is " << response_Time << " and its relative deadline is " << tasks[i].D << endl;
        if (response_Time > tasks[i].D) {
            return false;
        }
    }
    return true;
}

bool is_schedulable_edf(const vector<Task>& tasks) {
    double util = 0;

    for (size_t i = 0; i < tasks.size(); ++i) {
        util += static_cast<double> (tasks[i].C)/tasks[i].T;
    }
    //cout << "Util = " << util << endl;

    return util <= 1;
}

//Not need for HW1 - Run till 125000
double get_LCM(const vector<Task>& tasks) {
    return 0;
}

int main() {
    //Reading Aperiodic Tasks
    string filename = "aperiodic_tasks.csv";
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    int num_tasks;

    //Storing all tasks in Relative deadline order
    vector<Task> tasks;
    
    //HW2
    Task task1(3,7,7,0);
    task1.taskId = 0;
    Task task2(2,13,13,0);
    task2.taskId = 1;
    tasks.push_back(task1);
    tasks.push_back(task2);

    /* //Class Example
    Task task1(2,7,7,0);
    Task task2(1,15,15,0);
    tasks.push_back(task1);
    tasks.push_back(task2);
    */

    //Initalize the scheduler
    Scheduler S(tasks);


    //Read the AP tasks and populate in S
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        vector<int> values;
        string token;

        while (getline(ss, token, ',')) {
            int t = std::stoi(token);
            values.push_back(t);
        }

        //TaskID C I - Aperiodic Tasks format
        if (values.size() != 3) {
            continue; // Skip this line
        }

        Task task;
        task.taskId = values[0];
        task.C = values[1];

        task.T = 0;
        task.D = 0;
        task.I = values[2];

        //HW Q.2 & Q.3
        S.aperiodic_tasks.push_back(task);
    }

    file.close();

    int run_time = 15000; //Instead of LCM
    
    //Class Example
    /*
    Task A1 = Task(2,0,0,1);
    A1.taskId = 10;
    Task A2 = Task(3,0,0,7);
    A2.taskId = 11;
    Task A3= Task(3,0,0,9);
    A3.taskId = 12;
    */
    //HW2 Q.1
    /*
    Task A1 = Task(2,0,0,2);
    A1.taskId = 10;
    Task A2 = Task(1,0,0,6);
    A2.taskId = 11;
    Task A3= Task(2,0,0,9);
    A3.taskId = 12;
    Task A4= Task(1,0,0,15);
    A4.taskId = 13;



    S.aperiodic_tasks.push_back(A1);
    S.aperiodic_tasks.push_back(A2);
    S.aperiodic_tasks.push_back(A3);
    S.aperiodic_tasks.push_back(A4); //For HW2 Q.1 example

    //For Q.1
    S.SS = SporadicServer(4,12,12,0);
    S.schedule_rma = true; //Schedule via RMA
    S.setup_engine(run_time);
    */

    //For Q.2 and Q.3
    
    std::vector<Task>aperiodic_tasks_buffer = S.aperiodic_tasks;

    for(int period_idx = 2; period_idx <= 14; period_idx++) {

        S.reset();
        S.set_tasks(tasks);

        //Instantiating the Sporadic Server
        S.SS = SporadicServer(3,period_idx,period_idx,0);
        S.aperiodic_tasks = aperiodic_tasks_buffer;
        cout << "Num aperiodic Tasks: " << S.aperiodic_tasks.size()  << endl;

        int max_C = 0;

        for(int C = 1; C < S.SS.T; C++) {
            Task temp(C, S.SS.T, S.SS.T, 0);
            vector<Task> temp_tasks = tasks;
            temp_tasks.push_back(temp);

            sort(temp_tasks.begin(), temp_tasks.end(), comparePriorityRMA);
            if(is_schedulable_rma(temp_tasks)) {
                max_C = max(max_C, C);
            }
            else {
                cout << "Not schedulable for SS.T = " << S.SS.T << " and C = " << C << endl;
                break; //No other higher value of C is going to help here anyway
            }
        }

        if(max_C == 0) {    
            cout << "It's not possible to schedule this set for SS.T = " << S.SS.T << endl;
        }else {
            cout << "Schedule for SS.T = " << period_idx << " C = " << max_C << endl;

            S.SS.C = max_C;
            S.SS.capacity = max_C;
            S.schedule_rma = true; //Schedule via RMA
            S.setup_engine(run_time);

            cout << "Average response_time: " << (double)S.SS.total_resp_time/S.SS.num_processed << endl;
        }
    }


    return 0;
}
