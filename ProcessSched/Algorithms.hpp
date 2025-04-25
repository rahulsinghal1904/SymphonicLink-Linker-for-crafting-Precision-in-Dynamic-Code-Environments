#include "ProcessCreation.cpp"
#include <list>
#include <deque>
#include <string>
#include <vector>
using namespace std;

struct FinalSched {
private:
    deque<Process*> DoneQue; 
    Process *LiveProcess = nullptr;

    int TotalInputOutTime=0;
    int total_io_process_count;
    int InitiateInputOutTime;

protected:
    string type;
    list<Process*> QueProc;
    
public:
    bool prio_preempt = false;
    int unit;

    virtual void PutProcess(Process *p) = 0;

    virtual Process *FindComingProcess() = 0;


    void OutputProcQue();
    void printProcessDetails(Process* p);
    void calculateStatistics(int& FinishMaxT, double& UtilizationByCPU, double& UtilizationByInputOutput, double& ReturnAverage, double& MeanTimeWaiting, double& Return);
    void calculateMaxFinishTime(int& FinishMaxT);
    void calculateCPUUtilization(double& UtilizationByCPU, int FinishMaxT);

    void calculateIOUtilization(double& UtilizationByInputOutput, int FinishMaxT);
    void UpdateLiveProcess(Process *X);
    void calculateAvgTurnaround(double& ReturnAverage);

    Process *FindLiveProcess();
    void calculateAvgWaitTime(double& MeanTimeWaiting);
    void calculateThroughput(double& Return, int FinishMaxT);
    void Calcs();
    void add_process_with_decremented_priority(Process* X);
    void finish(Process *p, int DoneT);
    void InitiateInputOutput(int TimeNow);
    void ConcludeInputOutput(int TimeNow);

};

class FCFSScheduler: public FinalSched {
public:
    FCFSScheduler();
    void PutProcess(Process *p);
    Process *FindComingProcess();
};
class RoundRobin: public FinalSched{
public:
    RoundRobin(int unit);
    void PutProcess(Process *p);
    Process *FindComingProcess();

};
class LCSR : public FinalSched{
public:
    LCSR(const std::string& schedulerType);

    void PutProcess(Process *p);

    Process *FindComingProcess();
};

class PRIOScheduler : public FinalSched {
private:
    vector<list<Process*>> InAction; 
    vector<list<Process*>> DoneWith;
    void add_to_expired(Process *p);
    void add_to_active(Process *p);
    Process* get_next_from_active();
    void swap_queues();
public:
    PRIOScheduler(int unit, bool prio_preempt, int maxprio );
    void PutProcess(Process *p);
    Process* FindComingProcess();
};

