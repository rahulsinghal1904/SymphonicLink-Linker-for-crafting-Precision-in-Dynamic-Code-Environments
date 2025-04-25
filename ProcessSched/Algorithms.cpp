#include "Algorithms.hpp"
#include "ProcessCreation.cpp"
#include <iostream>
#include <iomanip>
using namespace std;
Process *FinalSched::FindLiveProcess() {
    return LiveProcess;
}
void FinalSched::UpdateLiveProcess(Process *X) {
    LiveProcess = X;
}

void FinalSched::add_process_with_decremented_priority(Process* X) {
    X->Rank = X->RankFixed - 1;
    PutProcess(X);
}


void FinalSched::OutputProcQue() {
    for (auto k = QueProc.begin(); k != QueProc.end(); ++k) {
        Process *X = *k;
        cout << X->giveOut() << endl;
    }
}

void FinalSched::printProcessDetails(Process* p) {
    cout << setfill('0') << right << setw(4) << p->Number << ": ";
    cout << p->ArrivalTime << " ";
    cout << p->TotalCPUDuration << " ";
    cout << p->CPUBurst << " ";
    cout << p->InputOutputTime << " ";
    cout << p->RankFixed << " | ";
    cout << p->TimeDone << " ";
    cout << p->TimeDone - p->ArrivalTime << " ";
    cout << p->TimeIO << " ";
    cout << p->WaitingCPU << endl;
}

void FinalSched::calculateMaxFinishTime(int& FinishMaxT) {
    for (auto&& p : DoneQue) {
        if (FinishMaxT < p->TimeDone) {
            FinishMaxT = p->TimeDone;
        }
    }
}

void FinalSched::calculateCPUUtilization(double& UtilizationByCPU, int FinishMaxT) {
    for (auto&& k : DoneQue) {
        UtilizationByCPU += k->TotalCPUDuration;
    }
    UtilizationByCPU = UtilizationByCPU * 100 / FinishMaxT;
}

void FinalSched::calculateIOUtilization(double& UtilizationByInputOutput, int FinishMaxT) {
    // cout<<TotalInputOutTime<<endl;
    UtilizationByInputOutput = TotalInputOutTime * 100.0 / FinishMaxT;
}

void FinalSched::calculateAvgTurnaround(double& ReturnAverage) {
    for (auto&& t : DoneQue) {
        ReturnAverage += t->TimeDone - t->ArrivalTime;
    }
    ReturnAverage = ReturnAverage / DoneQue.size();
}

void FinalSched::calculateAvgWaitTime(double& MeanTimeWaiting) {
    for (auto&& k : DoneQue) {
        MeanTimeWaiting += k->WaitingCPU;
    }
    MeanTimeWaiting = MeanTimeWaiting / DoneQue.size();
}

void FinalSched::calculateThroughput(double& Return, int FinishMaxT) {
    Return = 100.0 * DoneQue.size() / FinishMaxT;
}

void FinalSched::calculateStatistics(int& FinishMaxT, double& UtilizationByCPU, double& UtilizationByInputOutput, double& ReturnAverage, double& MeanTimeWaiting, double& Return) {
    for (auto&& t : DoneQue) {
        printProcessDetails(t);
    }
    calculateMaxFinishTime(FinishMaxT);
    calculateCPUUtilization(UtilizationByCPU, FinishMaxT);

    calculateIOUtilization(UtilizationByInputOutput, FinishMaxT);
    calculateAvgTurnaround(ReturnAverage);
    calculateAvgWaitTime(MeanTimeWaiting);
    calculateThroughput(Return, FinishMaxT);
}

void FinalSched::Calcs() {
    cout << type << endl;

    int FinishMaxT = 0;
    double UtilizationByCPU = 0;
    double UtilizationByInputOutput = 0;
    double ReturnAverage = 0;
    double MeanTimeWaiting = 0;
    double Return;

    calculateStatistics(FinishMaxT, UtilizationByCPU, UtilizationByInputOutput, ReturnAverage, MeanTimeWaiting, Return);

    cout << "SUM: " << FinishMaxT << " "
              << fixed << setprecision(2)
              << UtilizationByCPU << " "
              << UtilizationByInputOutput << " "
              << ReturnAverage << " "
              << MeanTimeWaiting << " "
              << fixed << setprecision(3)
              << Return << endl;
}

void FinalSched::finish(Process *p, int TimeDone) {
    p->TimeDone = TimeDone;
    for (auto k = DoneQue.begin(); k != DoneQue.end(); ++k) {
        if ((*k)->Number > p->Number) {
            DoneQue.insert(k, p);
            return;
        }
    }
    DoneQue.push_back(p); 
}

void FinalSched::InitiateInputOutput(int TimeNow) {
    total_io_process_count=total_io_process_count+1 ;
    if (total_io_process_count == 1) {
        InitiateInputOutTime = TimeNow;
    }
}

void FinalSched::ConcludeInputOutput(int currentTime) {

    --total_io_process_count;
    if (total_io_process_count == 0) {
            TotalInputOutTime += currentTime - InitiateInputOutTime;
        }
}

/**
 * FCFSScheduler
 */
FCFSScheduler::FCFSScheduler() {
    this->type = "FCFS";
}

void FCFSScheduler::PutProcess(Process *p) {
    unit = 10000;

    QueProc.push_back(p);
}

Process *FCFSScheduler::FindComingProcess() {
    unit = 10000;

    if (QueProc.empty()) {
        return nullptr;
    }
    
    Process *p = QueProc.front();
    QueProc.pop_front();
    return p;
}

/**
 * RoundRobin
 */
RoundRobin::RoundRobin(int unit) {
    this->unit = unit;
    this->type = "RR " + to_string(unit);
}

void RoundRobin::PutProcess(Process *p) {
    QueProc.push_back(p);
}

Process *RoundRobin::FindComingProcess() {
    if (QueProc.empty()) {
          return nullptr;
    }
    Process *p = QueProc.front();
    QueProc.pop_front();
    return p;
}

/**
 * LCFSscheduler
 */

LCSR::LCSR(const std::string& schedulerType)  {
this -> unit = 10000;
this-> type = schedulerType;
}

void LCSR::PutProcess(Process *p) {
    if (type == "LCFS") {
        QueProc.push_front(p);
    } else if (type == "SRTF") {
        auto k = QueProc.begin();
        for (; k != QueProc.end(); ++k) {
            if ((*k)->Left > p->Left) {
                break;
            }
        }
        QueProc.insert(k, p);
    }
}

Process *LCSR::FindComingProcess() {
    if (QueProc.empty()) {
        return nullptr;
    }

    Process *p = QueProc.front();
    QueProc.pop_front();
    return p;
}


/**
 * PRIOScheduler
 */
PRIOScheduler::PRIOScheduler(int unit,bool prio_preempt, int maxprio) {
    this->unit = unit;
    this->prio_preempt = prio_preempt;
for (int i = 0; i < maxprio; ++i) {
    InAction.push_back(list<Process*>());
    DoneWith.push_back(list<Process*>());
}
this->type = prio_preempt ? "PREPRIO " + to_string(unit) : "PRIO " + to_string(unit);

}

void PRIOScheduler::PutProcess(Process *p) {
    if (p->Rank < 0) {
        add_to_expired(p);
    } else {
        add_to_active(p);
    }
}

void PRIOScheduler::add_to_expired(Process *p) {
    p->Rank = p->RankFixed - 1;
    int idx = DoneWith.size() - p->Rank - 1;
    DoneWith[idx].push_back(p);
}

void PRIOScheduler::add_to_active(Process *p) {
    int idx = InAction.size() - p->Rank - 1;
    InAction[idx].push_back(p);
}


Process *PRIOScheduler::FindComingProcess() {
    Process *next_process = get_next_from_active();
    switch (static_cast<int>(next_process == nullptr)) {
        case 1:
            swap_queues();
            next_process = get_next_from_active();
            break;
        default:
            break;
    }


    return next_process;
}

Process *PRIOScheduler::get_next_from_active() {
    for (auto k = InAction.begin(); k != InAction.end(); ++k) {
        if (k->empty()) {
            
        } else{
            Process *p = k->front();
            k->pop_front();
            return p;
        }
    }
    return nullptr;
}

void PRIOScheduler::swap_queues() {
    InAction.swap(DoneWith);
}

