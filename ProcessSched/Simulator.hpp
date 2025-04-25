#include "EventControl.hpp"
#include "Randomizer.cpp"

Randomizer *R;
bool tellSchedulerBeInAction = false;

bool verbOut= false;
bool verbT = false;
bool verbE = false;
bool verbP = false;
using namespace std;

bool shouldCallScheduler() {
    return tellSchedulerBeInAction;
}

bool shouldSkipEvent(EventControl* eventScheduler, int TimeATM) {
    return eventScheduler->FindNextETime() == TimeATM;
}
void printProcessQueueIfNeeded(FinalSched* Director, bool verboseOutput) {
    if (verboseOutput) {
        Director->OutputProcQue();
    }
}

Process* getNextProcessAndUpdateLiveProcess(FinalSched* Director) {
    Process* nextProcess = Director->FindComingProcess();
    Director->UpdateLiveProcess(nextProcess);
    return nextProcess;
}

void putEventInScheduler(EventControl* eventScheduler, int TimeATM, Process* nextProcess) {
    if (nextProcess == nullptr) {
        return;
    }
        Event* newEvent = new Event(nextProcess,RUNNING, READY, TimeATM);
        eventScheduler->UpdateEvent(newEvent);
}

void handleNoCurrentProcess(FinalSched* Director, EventControl* eventScheduler, int TimeATM, bool verboseOutput) {
    printProcessQueueIfNeeded(Director, verboseOutput);
    
    Process* nextProcess = getNextProcessAndUpdateLiveProcess(Director);
    
    putEventInScheduler(eventScheduler, TimeATM, nextProcess);
}


void handleSchedulerCall(FinalSched* Director, EventControl* eventScheduler, int TimeATM, bool verboseOutput) {
    if (shouldCallScheduler()) {
        if (shouldSkipEvent(eventScheduler, TimeATM)) {
            return;
        }
        tellSchedulerBeInAction = false;
        if (Director->FindLiveProcess() == nullptr) {
            handleNoCurrentProcess(Director, eventScheduler, TimeATM, verboseOutput);
        }
    }
}

Event* createEvent(Process* X, STATE NextState, STATE eventState, int until) {
    return new Event(

        X,
        eventState,
        NextState,
        until
    );
}
void handleRemainingGreaterThanZero(Process* X, int TimeATM, int ioBurst, STATE NextState, FinalSched* Director, EventControl* eventScheduler) {
    Event* newEvent = new Event(
        X,
        READY,
        NextState,
        TimeATM + ioBurst

    );
    eventScheduler->UpdateEvent(newEvent);
    Director->InitiateInputOutput(TimeATM);
}

void handleRemainingLessThanOrEqualToZero(Process* X, int TimeATM, FinalSched* Director) {
    Director->finish(X, TimeATM);
}

void handleProcessRemaining(Process* X, int TimeATM, int ioBurst, STATE NextState, FinalSched* Director, EventControl* eventScheduler) {
   (X->Left > 0) ? 
    handleRemainingGreaterThanZero(X, TimeATM, ioBurst, NextState, Director, eventScheduler) : 
    handleRemainingLessThanOrEqualToZero(X, TimeATM, Director);
    Director->UpdateLiveProcess(nullptr);
}

void handleQuantumPreemption(Process* X, int BurstCPU, int TimeATM, FinalSched* MasterScheduler, EventControl* D, STATE NextState) {
    int Misc = MasterScheduler->unit;
    X->Left -= Misc;
    X->BurstCPU = BurstCPU - Misc;

    int until = TimeATM + Misc;
    D->UpdateEvent(createEvent(X, NextState, READY, until));
}

void handleNoQuantumPreemption(Process* X, int BurstCPU, int TimeATM, EventControl* D, STATE NextState) {
    int Misc = BurstCPU;
    X->Left -= Misc;
    X->BurstCPU = 0;

    int until = TimeATM + Misc;
    D->UpdateEvent(createEvent(X, NextState, BLOCKED, until));
}

void handleCPUburst(Process* X, int BurstCPU, int TimeATM, FinalSched* MasterScheduler, EventControl* D, STATE NextState) {
 (BurstCPU > MasterScheduler->unit) ? 
    handleQuantumPreemption(X, BurstCPU, TimeATM, MasterScheduler, D, NextState) : 
    handleNoQuantumPreemption(X, BurstCPU, TimeATM, D, NextState);

}

bool isPreemptionEnabledAndRunning(FinalSched* MasterScheduler, Process* LiveProcess) {
    return MasterScheduler->prio_preempt && LiveProcess;
}

Event* getPendingEvent(EventControl* D, int processNumber) {
    return D->FindNextEvent(processNumber);
}

bool shouldPreemptBasedOnEventAndPriority(Event* LeftEvt, int TimeNow, Process* LiveProcess, Process* X) {
  return (LeftEvt && LiveProcess) &&
       (LeftEvt->Instant > TimeNow) &&
       (X->Rank > LiveProcess->Rank);

}

bool shouldPreempt(FinalSched* MasterScheduler, Process* LiveProcess, EventControl* D, int TimeNow, Process* X) {
    if (!isPreemptionEnabledAndRunning(MasterScheduler, LiveProcess))
        return false;

    Event* LeftEvt = getPendingEvent(D, LiveProcess->Number);
    
    return shouldPreemptBasedOnEventAndPriority(LeftEvt, TimeNow, LiveProcess, X);
}

int calculate_cpu_burst(Process* X, FinalSched* MasterScheduler, Randomizer* R) {
 Process* ProcessLive = MasterScheduler->FindLiveProcess();
    int BurstCPU = ProcessLive->BurstCPU > 0 ? ProcessLive->BurstCPU : R->getRandomValue(X->CPUBurst);
    return std::min(BurstCPU, ProcessLive->Left);
}

void update_cpu_waiting_time(Process* X, int PrevStateTime) {
    X->WaitingCPU += PrevStateTime;
}


void updateProcessState(Process* X, int LeftTime, int UpdatedCBurst) {
    X->Left = LeftTime;
    X->BurstCPU = UpdatedCBurst;
}

void removeEventAndUpdateState(Process* X, EventControl* D, int TimeNow) {
    Event* LeftEvt = D->FindNextEvent(X->Number);
    int PrevLeftTime = X->Left;
    int LeftTime = PrevLeftTime + LeftEvt->Instant - TimeNow;
    int UpdatedCBurst = X->BurstCPU + LeftEvt->Instant - TimeNow;
    D->DeleteEvent(X->Number);
    updateProcessState(X, LeftTime, UpdatedCBurst);
}

void scheduleNewEvent(Process* X, EventControl* D, int TimeNow) {
    Event* event = new Event(X, READY, RUNNING, TimeNow);
    D->UpdateEvent(event);
}

void preemptCurrentProcess(Process* LiveProcess, EventControl* D, int TimeNow) {
    removeEventAndUpdateState(LiveProcess, D, TimeNow);
    scheduleNewEvent(LiveProcess, D, TimeNow);
}
void adjust_priority_and_schedule(Process* X, FinalSched* MasterScheduler) {
    X->Rank = X->Rank - 1;

    MasterScheduler->UpdateLiveProcess(nullptr);

    MasterScheduler->PutProcess(X);
}
class Simulator {
private:
    EventControl *eventControl;
    FinalSched *masterScheduler;

public:
    Simulator(EventControl *ec, FinalSched *ms) : eventControl(ec), masterScheduler(ms) {}

    void initializeEventDetails(Event* evt, Process*& X, int& TimeNow, STATE& PrevState, STATE& NextState, int& StateTransitionDuration, int& PrevStateTime) {
    X = evt->X;
    TimeNow = evt->Instant;
    PrevState = evt->PrevState;
    NextState = evt->NextState;
}


    void run() {
        Event *evt;
        while ((evt = eventControl->FindNextEvent())) {
            Process *X;
            int TimeNow;
            STATE PrevState;
            STATE NextState;
            int StateTransitionDuration;
            int PrevStateTime;
            initializeEventDetails(evt, X, TimeNow, PrevState, NextState, StateTransitionDuration, PrevStateTime);
            StateTransitionDuration = X->TransitionTime;
            PrevStateTime = TimeNow - StateTransitionDuration;

            X->TransitionTime = TimeNow;
            if (verbOut) {
                if (verbalizeState(NextState) == "READY") {
                    std::cout << TimeNow << " " << X->Number << " " << PrevStateTime << ": " << verbalizeState(PrevState) << " -> " << verbalizeState(NextState) << std::endl;
                } else {
                    std::cout << TimeNow << " " << X->Number << " " << PrevStateTime << ": " << verbalizeState(PrevState) << " -> " << verbalizeState(NextState);
                }
            }

            auto processBlockedState = [&]() { X->TimeIO += PrevStateTime, masterScheduler->ConcludeInputOutput(TimeNow); };

            delete evt;
            evt = nullptr;

            if (NextState == READY) {
                if (PrevState == BLOCKED || PrevState == CREATED) {
                    if (PrevState == BLOCKED)
                        processBlockedState();

                    masterScheduler->add_process_with_decremented_priority(X);

                    Process *LiveProcess = masterScheduler->FindLiveProcess();
                    if (shouldPreempt(masterScheduler, LiveProcess, eventControl, TimeNow, X)) {
                        preemptCurrentProcess(LiveProcess, eventControl, TimeNow);
                    }
                } else if (PrevState == RUNNING) {
                    adjust_priority_and_schedule(X, masterScheduler);
                }
                tellSchedulerBeInAction = true;
            }
            else if (NextState == RUNNING) {
                int BurstCPU = calculate_cpu_burst(X, masterScheduler, R);
                update_cpu_waiting_time(X, PrevStateTime);

                if (verbOut) {
                    std::cout << " CPUBurst=" << BurstCPU << " Left=" << X->Left << " prio=" << X->Rank << '\n';
                }

                handleCPUburst(X, BurstCPU, TimeNow, masterScheduler, eventControl, NextState);
            }
            else if (NextState == BLOCKED) {
                int BurstIO = X->Left > 0 ? R->getRandomValue(X->InputOutputTime) : 0;

                if (verbOut) {
                    std::cout << " ib=" << BurstIO << " Left=" << X->Left << '\n';
                }

                handleProcessRemaining(X, TimeNow, BurstIO, NextState, masterScheduler, eventControl);
                tellSchedulerBeInAction = true;
            }
            
            handleSchedulerCall(masterScheduler, eventControl, TimeNow, verbOut);
        }
        masterScheduler->Calcs();
    }
};