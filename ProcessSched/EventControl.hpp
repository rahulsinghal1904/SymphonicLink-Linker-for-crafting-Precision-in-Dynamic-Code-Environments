#include "Algorithms.hpp"
#include <unordered_map>
#include <string>
#include <algorithm>
using namespace std; 


enum STATE {
    BLOCKED,
    CREATED,
    READY,
    RUNNING,
};


string verbalizeState(STATE s);

class Event {
public:

    STATE NextState;
    Process *X;
    STATE PrevState;
    int Instant;


    Event(Process* X, STATE NextState, STATE PrevState, int Instant)
    : X(X), Instant(Instant), PrevState(PrevState), NextState(NextState) {}


    string giveOut() {
    ::string ret("Event, time: ");
    ret += ::to_string(Instant) + " X: " + ::to_string(X->Number) + " old: " + verbalizeState(PrevState) + " new: " + verbalizeState(NextState);
    return ret;
}
};

class EventControl {
private:
    list<Event*> eve_q;

public:
void UpdateEvent(Event* event) {
    auto k = find_if(eve_q.begin(), eve_q.end(),
                           [event](Event* e) { return event->Instant < e->Instant; });
    eve_q.insert(k, event);
}

 Event* FindNextEvent() {
    Event* e = nullptr;
    return !eve_q.empty() ? (e = eve_q.front(), eve_q.pop_front(), e) : nullptr;
}

Event* FindNextEvent(int pid) {
    for (Event* e : eve_q) {
        if (e->X->Number == pid) {
            return e;
        }
    }
    return nullptr;
}

void DeleteEvent(int ProcessID) {
    eve_q.erase(remove_if(eve_q.begin(), eve_q.end(),
                          [ProcessID](Event* e) { return e->X->Number == ProcessID; }),
                          eve_q.end());
}


  
    int FindNextETime() {
    return eve_q.empty() ? -1 : eve_q.front()->Instant;
}
};
