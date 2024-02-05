#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <set>
using namespace std;
//TODO: Remove set and use heap
//Add Policy fields
//Move classes to their own files
//Add instance ID
//Derive class for instance

//template<typename Z>
struct Task {
    int taskId;
    int C; //Worst Case ComputaTon Tme
    int T; //Tme Period
    int D; //Relative Deadline = T (for LL tasks)
    int I; //Phase

    int processed_time  = 0; //Used for instances //TODO: Add Instance class derived from Task
    int numInstances    = 0;
    int instanceId      = 0;

    bool operator<(const Task& Task2) const {
        return D < Task2.D;    //Scheduler's Set uses this to sort tasks
    }

    Task(int _C, int _T, int _D, int _I) : C(_C), T(_T), D(_D), I(_I) {}
    Task() : C(0), T(0), D(0), I(0) {}
};


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


class Scheduler {
public:
    vector<Task> s_tasks;   //Store all tasks
    set<Task> available_tasks;

    int s_clock = 0;    //Clock counter;

    void clock_rma() {
        //Insert any new tasks for this clock
        for(int idx = 0; idx < s_tasks.size(); idx++) {
            if((s_clock - s_tasks[idx].I)%s_tasks[idx].T == 0) {
                Task I(s_tasks[idx].C, s_tasks[idx].T, s_tasks[idx].D, s_tasks[idx].I);
                I.taskId = s_tasks[idx].taskId;
                available_tasks.insert(I);
                cout << s_clock << ": Adding Task " << idx << endl;
            }
        }

        /*for(auto s: available_tasks) {
            cout << s_clock << ":Order: Task " << s.taskId << endl;
        }*/

        //Process tasks for this clock
        if(!available_tasks.empty()) {
            Task winner_task = *(available_tasks.begin());
            winner_task.processed_time++;
            cout << s_clock << " " << winner_task.taskId << " " << winner_task.instanceId << endl;

            //Since we cannot update objects inside a set, just removed and add a new one
            available_tasks.erase(available_tasks.begin());
            available_tasks.insert(winner_task);

            //If done processing then remove
            if(winner_task.processed_time == winner_task.C) {
                cout << s_clock << ":Removing Task " << winner_task.taskId << endl;
                available_tasks.erase(available_tasks.begin());
            }
        }
        //Check that no 2 instances have the same taskId i.e. no 2 instances of a task are present at a given moment
        s_clock++;
    }

    void process_rma(int max_clock) {
        for(int idx = 0; idx < max_clock; idx++) clock_rma();
    }

    Scheduler(vector<Task>_tasks) : s_tasks(_tasks) {}
};


int main() {
    //Reading Tasks
    string filename = "tasks_q1.csv";
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
        S.process_rma(20);
    } else {
        cout << "The tasks are not schedulable according to Rate-Monotonic Analysis (RMA)." << endl;
    }

    return 0;
}
