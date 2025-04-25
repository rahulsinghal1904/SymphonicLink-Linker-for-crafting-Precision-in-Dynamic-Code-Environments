#include "EventControl.hpp"
#include <unordered_map>
using namespace std; 

string verbalizeState(STATE s) {
     static const std::unordered_map<STATE, std::string> MapState {
        {CREATED, "CREATED"},
        {READY, "READY"},
        {RUNNING, "RUNNING"},
        {BLOCKED, "BLOCKED"}
    };

    auto A = MapState.find(s);
    if (A != MapState.end()) {
        return A->second;
    } else {
        return "Unknown State";
    }
}