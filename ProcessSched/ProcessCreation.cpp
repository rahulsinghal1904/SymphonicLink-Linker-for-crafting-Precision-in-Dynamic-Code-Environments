
#ifndef PROCESS_CREATION_H
#define PROCESS_CREATION_H
#include <string>
using namespace std;


struct Process {
public:

    int CPUBurst;
    int WaitingCPU;
    int Left;
    int TotalCPUDuration;
    int TimeDone;
    int TimeIO;
    int InputOutputTime;
    int BurstCPU;
    int Rank;
    int RankFixed;
    int Number;
    int ArrivalTime;
    int TransitionTime;
    int BurstIO;

   

Process(int InputOutputTime, int Number, int ArrivalTime, int CPUBurst, int TotalCPUDuration, int RankFixed) {
        this->Number = Number;
        this->ArrivalTime = ArrivalTime;
        this->TotalCPUDuration = TotalCPUDuration;
        this->CPUBurst = CPUBurst;
        this->InputOutputTime = InputOutputTime;
        this->RankFixed = RankFixed;
        this->BurstIO = 0;
        this->TransitionTime = 0;
        this->BurstCPU = 0;
        this->WaitingCPU = 0;
        this->Rank = RankFixed - 1;
        this->Left = TotalCPUDuration;
        this->TimeIO = 0;
    }



    string giveOut() const {
    return "Process Information - ID: " + ::to_string(Number) + ", Remaining Time: " + ::to_string(Left) 
         + ", CPU Burst: " + ::to_string(BurstCPU) + ", InputOutputTime Burst: " + ::to_string(BurstIO);
}


};

#endif 