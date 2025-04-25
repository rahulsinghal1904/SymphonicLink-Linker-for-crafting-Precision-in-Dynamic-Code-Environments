#include "iostream"
#include "fstream"
#include "string.h"
#include <map>
#include "Simulator.hpp"
#include <sstream>

using namespace std;



void parseArguments(int argc, char* argv[], vector<string>& files, string& MasterScheduler, bool& verbOut, bool& verbT, bool& verbE, bool& verbP) {
    int i = 1;
    while (i < argc) {
        string arg = string(argv[i]);
        if (arg.size() < 2) {
            files.push_back(arg);
            ++i;
            continue;
        }

        switch (arg[0]) {
            case '-':
                switch (arg[1]) {
                    case 's':
                        MasterScheduler = arg.substr(2);
                        break;
                    case 'v':
                        verbOut= true;
                        break;
                    case 't':
                        verbT = true;
                        break;
                    case 'e':
                        verbE = true;
                        break;
                    case 'p':
                        verbP = true;
                        break;
                    default:
                        files.push_back(arg);
                        break;
                }
                break;
            default:
                files.push_back(arg);
                break;
        }
        ++i;
    }
}
Process* createProcess(int newProcessNumber, int arrivalTime, int burstTime, int Rank, int remainingTime, Randomizer* R, int m) {
    Process* ProcN = new Process(remainingTime, newProcessNumber, arrivalTime, Rank, burstTime, R->getRandomValue(m));
    
    return ProcN;
}



int main(int argc, char *argv[]) {
  if (argc < 3) {
    cout << "Usage: " << argv[0] << " [-v] [-t] [-e] [-p] [-s <schedspec>] <inputfile> <randfile>" << endl;
    cout << "Options:" << endl;
    cout << "  -v                  : Enable verbose mode" << endl;
    cout << "  -t                  : Enable verbose trace mode" << endl;
    cout << "  -e                  : Enable verbose event queue mode" << endl;
    cout << "  -p                  : Enable verbose preempt mode" << endl;
    cout << "  -s <schedspec>      : Specify scheduler" << endl;
    return 1;
}




    vector<string> files;
    string MasterScheduler;

    parseArguments(argc, argv, files, MasterScheduler, verbOut, verbT, verbE, verbP);

    fstream in;
    in.open(files[0].c_str());
    R = new Randomizer(files[1]);

    int q = 0;
    int m = 4;
    FinalSched* s = nullptr;

    char C = MasterScheduler[0];
    size_t idx = MasterScheduler.find(':');

    switch (C) {
        case 'F':
            s = new FCFSScheduler();
            break;
        case 'L':
            s = new LCSR("LCFS");
            break;
        case 'S':
            s = new LCSR("SRTF");
            break;
        case 'R':
        case 'P':
        case 'E':
            if (idx != string::npos) {
                q = stoi(MasterScheduler.substr(1, idx));
                m = stoi(MasterScheduler.substr(idx + 1));
            } else {
                q = stoi(MasterScheduler.substr(1));
            }
            if (C == 'R') {
                s = new RoundRobin(q);
            } else {
                bool e = (C == 'E');
                s = new PRIOScheduler(q, e, m);
            }
            break;
        default:
            break;
    }

    EventControl *des = new EventControl();
    char line[100];
    int Process_N = 0;

    for (string line; getline(in, line); ) {
        istringstream iss(line);
        int temp[4], i = 0;
        for (string token; iss >> token; ) {
            stringstream ss(token);
            ss >> temp[i++];
        }


   int newProcessNumber = Process_N++;
    int arrivalTime = temp[0];
    int burstTime = temp[1];
    int Rank = temp[2];
    int remainingTime = temp[3];

Process* newProcess = createProcess(newProcessNumber, arrivalTime, burstTime, Rank, remainingTime, R, m);

Event* newEvent = new Event(newProcess, READY, CREATED, newProcess->ArrivalTime);

des->UpdateEvent(newEvent);

    }


    in.close();
    Simulator simulator(des, s);

    simulator.run();


    return 0;
}