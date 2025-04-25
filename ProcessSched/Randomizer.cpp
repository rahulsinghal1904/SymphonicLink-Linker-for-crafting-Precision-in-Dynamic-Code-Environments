#include <fstream>
#include <deque>
#include <string>
#include <stdexcept>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <algorithm>
using namespace std;

class Randomizer {
private:
    deque<int> RandVals;
    size_t CharPos = 0;

    void readRandomValuesFromFile(const string& file_path) {
        ifstream file(file_path);
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + file_path);
        }
        int value;
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        while (file >> value) {
            RandVals.push_back(value);
        }
        file.close();
    }

public:
    Randomizer(const string& file_path) {
        srand(static_cast<unsigned int>(time(nullptr))); 
        readRandomValuesFromFile(file_path);
    }
  int getRandomValue() {

        if (CharPos == RandVals.size()) {
            CharPos = 0;
        }
        int value = RandVals[CharPos++];
        return value;
    }
    int getRandomValue(int num) {
        if (num <= 0) {
            throw invalid_argument("Invalid argument: num must be greater than zero");
        }
        
        return getRandomValue() % num + 1;
    }
};
