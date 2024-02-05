#ifndef _TASK_H_
#define _TASK_H_

struct Task {
    int taskId;
    int C; //Worst Case ComputaTon Tme
    int T; //Tme Period
    int D; //Relative Deadline = T (for LL tasks)
    int I; //Phase
    int numInstances    = 0;

    bool operator<(const Task& Task2) const {
        return D < Task2.D;    //Scheduler's Set uses this to sort tasks
    }

    Task(int _C, int _T, int _D, int _I) : C(_C), T(_T), D(_D), I(_I) {}
    Task() : C(0), T(0), D(0), I(0) {}
};

struct Instance: public Task {
    int processed_time  = 0; //Used for instances //TODO: Add Instance class derived from Task
    int instanceId      = 0;
    
    Instance(int _C, int _T, int _D, int _I) : Task(_C,_T,_D,_I) {}
    Instance() : Task() {}
};

#endif