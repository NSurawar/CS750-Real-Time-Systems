#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <set>
using namespace std;
#include "/Users/neerajsurawar/git/CS750/HW1/Task.h"
#include "/Users/neerajsurawar/git/CS750/HW1/Scheduler.h"

//TODO: Remove set and use heap
//Add Policy fields
//Move classes to their own files
//Add instance ID
//Derive class for instance

//Ignore Phase since worst case response Tme is when all tasks are scheduled at the same Tme.
double calculate_response_Time(int task_index, const vector<Task>& tasks) {
    double response_Time = tasks[task_index].C;

    //Assuing tasks are sorted based on their relaTve deadlines - Only check for indexes lower than yours
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
    //Reading Tasks
    string filename = "TaskSet2.csv";
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    int num_tasks;
    file >> num_tasks;

    //cout << "Num Tasks: " << num_tasks << endl;

    //Storing all tasks in Relative deadline order
    vector<Task> tasks;
    
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        vector<int> values;
        string token;

        while (getline(ss, token, ',')) {
            values.push_back(stoi(token));
        }

        if (values.size() != 5) {
            continue; // Skip this line
        }

        Task task;
        task.taskId = values[0];
        task.C = values[1];
        task.T = values[2];
        task.D = values[3];
        task.I = values[4];

        tasks.push_back(task);
    }

    file.close();

    //Put tasks in relaTve deadline order
    /*vector<Task> tasks = {
        {95, 1250, 1250, 0},
        {188, 1563, 1563, 500},
        {1072, 3906, 3906, 700},
        {2144, 7812, 7812, 300},
        {1072, 62500, 62500, 120},
        {893, 125000, 125000, 0}
    };*/

    int run_time = 125000; //Instead of LCM
    
    //Instantiating the Scheduler
    Scheduler S(tasks);

    if (is_schedulable_rma(tasks)) {
        cout << "The tasks are schedulable according to Rate-Monotonic Analysis (RMA)." << endl;
        S.schedule_rma = true; //Schedule via RMA
        S.setup_engine(run_time);
    }
    else {
        cout << "The tasks are not schedulable according to Rate-Monotonic Analysis (RMA)." << endl;
    }

    if (is_schedulable_edf(tasks)) {
        cout << "The tasks are schedulable according to Early Deadline First (EDF)" << endl;
        S.schedule_rma = false; //Schedule via EDF
        S.s_clock = 0;
        S.setup_engine(run_time);
    }
    else {
        cout << "The tasks are not schedulable according to Early Deadline First (EDF)" << endl;
    }


    return 0;
}
