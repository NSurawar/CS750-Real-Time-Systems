#include <iostream>
#include <fstream>
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

double get_LCM(const vector<Task>& tasks) {
    return 0;
}


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
        cout << prev_response_Time << " : " << response_Time << endl;
        if(response_Time == prev_response_Time || response_Time > tasks[task_index].D) break;
    }

    return response_Time;
}

bool is_schedulable(const vector<Task>& tasks) {
    for (size_t i = 0; i < tasks.size(); ++i) {
        double response_Time = calculate_response_Time(i, tasks);
        std::cout << "Worst-Case Response time for Task " << i << " is " << response_Time << "\tand its relative deadline is " << tasks[i].D << endl;
        if (response_Time > tasks[i].D) {
            return false;
        }
    }
    return true;
}

int main() {
    //Reading Tasks
    string filename = "tasks.csv";
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    int num_tasks;
    file >> num_tasks;
    cout << "Num Tasks: " << num_tasks << endl;

    //Storing all tasks in Relative deadline order
    vector<Task> tasks(num_tasks);

    for (int i = 0; i < num_tasks; ++i) {
        file >> tasks[i].taskId >> tasks[i].C >> tasks[i].T >> tasks[i].D >> tasks[i].I;
    }

    file.close();


    for (int i = 0; i < num_tasks; ++i) {
        cout << tasks[i].taskId << " " << tasks[i].C << " " << tasks[i].T << " " << tasks[i].D << " " << tasks[i].I << endl;
    }

    //Put tasks in relaTve deadline order
    /*vector<Task> tasks = {
        {95, 1250, 1250, 0},
        {188, 1563, 1563, 500},
        {1072, 3906, 3906, 700},
        {2144, 7812, 7812, 300},
        {1072, 62500, 62500, 120},
        {893, 125000, 125000, 0}
    };*/

    if (is_schedulable(tasks)) {
        cout << "The tasks are schedulable according to Rate-Monotonic Analysis (RMA)." << endl;
        Scheduler S(tasks);
        S.schedule_rma = false;
        S.setup_engine(20);
    } else {
        cout << "The tasks are not schedulable according to Rate-Monotonic Analysis (RMA)." << endl;
    }

    return 0;
}
