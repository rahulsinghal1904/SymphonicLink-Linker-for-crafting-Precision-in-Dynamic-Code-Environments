#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstring>
#include <set>
#include <list>
#include <algorithm>


using namespace std;
int lCounter=0;
int currPos=0;
ssize_t lSize=0;

class Module {
private:
    int bs_addr;
    int defcnt;
    int usecnt;
    int codecnt;

public:
    Module(int base=0, int def=0, int use=0, int code=0) : bs_addr(base), defcnt(def), usecnt(use), codecnt(code) {}

    int getBaseAddress() const { return bs_addr; }
    int getDefCount() const { return defcnt; }
    int getUseCount() const { return usecnt; }
    int getCodeCount() const { return codecnt; }
    
    void setBaseAddress(int base) { bs_addr = base; }
    void setDefCount(int def) { defcnt = def; }
    void setUseCount(int use) { usecnt = use; }
    void setCodeCount(int code) { codecnt = code; }
};


class Symbol {

private:
    std::string word;
    int location;
    bool multipleDefined;
    bool inUse= false;
    bool utilized;
    int module;
    bool flag;


public:
    std::string getWord() const {
        return word;
    }

    void setWord(const std::string& newWord) {
        word = newWord;
    }

    int getLocation() const {
        return location;
    }

    void setLocation(int newLocation) {
        location = newLocation;
    }

    bool isMultipleDefined() const {
        return multipleDefined;
    }

    void setMultipleDefined(bool newMultipleDefined) {
        multipleDefined = newMultipleDefined;
    }

    bool isInUse() const {
        return inUse;
    }

    void setInUse(bool newInUse) {
        inUse = newInUse;
    }

    bool isUtilized() const {
        return utilized;
    }

    void setUtilized(bool newUtilized) {
        utilized = newUtilized;
    }

    int getModule() const {
        return module;
    }

    void setModule(int newModule) {
        module = newModule;
    }
};

vector<Module> modules;
vector<string> K;
vector < pair < string, Symbol > > symbol_table;

void makeSym(const std::string& word, int location, bool multipleDefined, bool inUse, bool utilized, int module) {
  auto it = std::lower_bound(symbol_table.begin(), symbol_table.end(), word,
                           [](const std::pair<std::string, Symbol>& sym, const std::string& key) {
                               return sym.first < key;
                           });

    if (it == symbol_table.end() || it->first != word) { 
        Symbol symbol;
        symbol.setWord(word);
        symbol.setLocation(location);
        symbol.setModule(module);
        symbol.setMultipleDefined(false); 
        symbol.setInUse(false);
        symbol.setUtilized(false);
        symbol_table.insert(it, std::make_pair(word, symbol));    } 
        else {
        it->second.setMultipleDefined(true); 
     
                cout<<"Warning: Module "<<module-1<<": "<<word.c_str()<<" redefinition ignored"<<endl;
    

    }
}




class ExtractTokenClass {
private:
    FILE* fileStream;
    char* line;
    char* lineStart;

    size_t lWidth;
    ssize_t lineSupport;
    int finalLineNum;
    int finalOffset;
    const char* delim;

public:
    ExtractTokenClass() : line(NULL), lineStart(NULL), lWidth(0), lineSupport(0),finalLineNum(0), finalOffset(0), delim(" \t\n") {}

    ~ExtractTokenClass() {

    }

    FILE* openFile(const char* filename) {
        fileStream = fopen(filename, "r");
        if (!fileStream) {
            cout << "Error opening file: " << filename << endl;
            exit(EXIT_FAILURE);
        }
        return fileStream;
    }


    char* extractToken() {
       
        char *ptrMain;

        ptrMain = strtok(NULL, delim);
        
        while (ptrMain == NULL && !feof(fileStream)) {
            if ((lineSupport = getline(&line, &lWidth, fileStream)) > 0) {
                lCounter++;
                lSize=lineSupport;
                lineStart=line;
                ptrMain = strtok(line, delim);
                
            }
        }
        if (ptrMain != NULL) {
           currPos =(ptrMain - lineStart);
           currPos = currPos+1;

        } else {
            currPos = lSize;
            finalLineNum = lCounter;
            finalOffset = currPos;
        }

        return ptrMain;
    }

    int getLineNumber() const {
        return lCounter;
    }

 
    int getFinalLineNum() const {
        return finalLineNum;
    }

    int getFinalOffset() const {
        return finalOffset;
    }

};



void parseerror(int errcode, int lCounter, int lineoffset) {
    static const char* errstr[] = {
        "NUM_EXPECTED",              // Number expect, anything >= 2^30 is not a number either
        "SYM_EXPECTED",              // Symbol Expected
        "MARIE_EXPECTED",            // Addressing Expected which is M/A/R/I/E
        "SYM_TOO_LONG",              // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE",    // > 16
        "TOO_MANY_USE_IN_MODULE",    // > 16
        "TOO_MANY_INSTR",            // total num_instr exceeds memory size (512)
    };
    printf("Parse Error line %d offset %d: %s\n", lCounter, lineoffset, errstr[errcode]);
    exit(1);
}

char* readSymbol(ExtractTokenClass& tokenizer) {
    char* token;

    token = tokenizer.extractToken();

    if (token == nullptr) {
            
          parseerror(1,tokenizer.getLineNumber(), currPos);

           exit(0);
    }  else if (strlen(token) > 16) {
        parseerror(3,  tokenizer.getLineNumber(), currPos);
        exit(0);
    }

    if (!isalpha(token[0])){
          parseerror(1,  tokenizer.getLineNumber(), currPos);
           exit(0);
    }

    for (size_t i = 1; i < strlen(token); ++i) {
        if (!isalnum(token[i])){
              parseerror(1,  tokenizer.getLineNumber(), currPos);
              exit(0);
        }
    }

    return token;
}

int readInt (ExtractTokenClass& tokenizer){

    char* token;    
    token = tokenizer.extractToken();
    if (token == nullptr) {
        return -1; 
    }
    
    char* ptr = token;
     while (*ptr != '\0') {
        char ch = *ptr;
        if (!isdigit(ch)) {
            parseerror(0, tokenizer.getLineNumber(), currPos);
            exit(0);
        }
         ptr++; 
     }
    
    try {
        return stoi(token);
    } catch (const invalid_argument&) {
        parseerror(0, tokenizer.getLineNumber(), currPos);
        exit(0);
    } catch (const out_of_range&) {
        parseerror(0, tokenizer.getLineNumber(), currPos);
        exit(0);
    }
}

string readMARIE(ExtractTokenClass& tokenizer) {
    char* token;
    
    token = tokenizer.extractToken();

   if (token == nullptr || strlen(token) != 1 || (token[0] != 'M' && token[0] != 'A' && token[0] != 'R' && token[0] != 'I' && token[0] != 'E')) {
    parseerror(2, tokenizer.getLineNumber(), currPos);
    exit(0);
}


    return token;
}


void readDefList(int defcount, Module& module, vector<Symbol>& deflist, ExtractTokenClass& tokenizer, int module_num) {
    for (int i = 0; i < defcount; i++) {
        string symbol = readSymbol(tokenizer);
        
        K.push_back(symbol);

        int val = readInt(tokenizer);
        Symbol s;
        s.setWord(symbol);
        s.setLocation(val);
        s.setModule(module_num + 1);
        deflist.push_back(s);
    }
}

void CheckDefCountOutOfRange(ExtractTokenClass& tokenizer, int module_num) {
    parseerror(4, tokenizer.getLineNumber(), currPos);
    exit(0);
}
enum UseCountClassification {
    ValidUseCount,
    ExceededUseCount,
    InvalidUseCount
};

UseCountClassification CategorizeUseCount(int use_number) {
    if (use_number >= 0 && use_number <= 16)
        return ValidUseCount;
    else if (use_number > 16)
        return ExceededUseCount;
    else
        return InvalidUseCount;
}

void processValidUseCount(int use_number, Module& module, ExtractTokenClass& tokenizer) {
    module.setUseCount(use_number);
    for (int i = 0; i < use_number; i++) {
        string symbol = readSymbol(tokenizer);

    }
}

void CheckUseCountOutOfRange(ExtractTokenClass& tokenizer) {
    parseerror(5, tokenizer.getLineNumber(), currPos);
    exit(0);
}


void CheckCodeCountOutOfRange(int module_num, const Symbol& s, int val, int codecount) {
    printf("Warning: Module %d: %s=%d valid=[0..%d] assume zero relative\n", module_num, s.getWord().c_str(), val, codecount - 1);
}

bool MultipleDefinitionWarning = false;
int moduleMultDef;
string symbolMultDef;

void printSymbolEntry() {
    std::vector<std::string> printedEntries; 

    
    for (const auto& key : K) {
        auto it = std::find_if(symbol_table.begin(), symbol_table.end(),
                               [&key](const std::pair<std::string, Symbol>& entry) {
                                   return entry.first == key;
                               });
        if (it != symbol_table.end()) {
            const auto& entry = *it;
            const auto& word = entry.first;
            const auto& symbol = entry.second;

            if (std::find(printedEntries.begin(), printedEntries.end(), word) == printedEntries.end()) {
                if (!symbol.isMultipleDefined()) {
                    printf("%s=%d\n", word.c_str(), symbol.getLocation());
                } else {
                    printf("%s=%d Error: This variable is multiple times defined; first value used\n", word.c_str(), symbol.getLocation());
                }

                printedEntries.push_back(word); 
            }
        }
    }
}

        
void processMARIEInstruction(ExtractTokenClass& tokenizer) {

    string addressmode = readMARIE(tokenizer);

    int instr = readInt(tokenizer);
}


void PassOne(const char* input_file) {
    FILE *file;
    ExtractTokenClass tokenizer;
    file=tokenizer.openFile(input_file);

    int whole_length = 0;
    int module_num = 0;
    int charPs = 0;
    while(!feof(file)){
            
            Module module;
            vector<Symbol> deflist;

            int defcount = readInt(tokenizer);

            if (defcount >= 0 && defcount <= 16) {
                module.setDefCount(defcount);
                readDefList(defcount, module, deflist, tokenizer, module_num);
            } else if (defcount > 16) {
                CheckDefCountOutOfRange(tokenizer, module_num);

            }
  

            int usecount = readInt(tokenizer);

            switch (CategorizeUseCount(usecount)) {
                case ValidUseCount:
                    processValidUseCount(usecount, module, tokenizer);
                    break;
                case ExceededUseCount:
                    CheckUseCountOutOfRange(tokenizer);
                    break;
                default:
                    break;
            }

        int codecount = readInt(tokenizer);

        whole_length = whole_length+ codecount;
        if(codecount >= 0 && whole_length <= 512){
            module.setCodeCount(codecount);
            module.setBaseAddress(charPs);
           
            for (int i = 0; i < codecount; i++) {

            processMARIEInstruction(tokenizer);


            }
            charPs=charPs+codecount;
            modules.push_back(module);

        }
        else if(whole_length > 512){

            parseerror(6,tokenizer.getLineNumber(), currPos);
            exit(0);
        }

        for (const auto& s : deflist) {
            int val = s.getLocation();

            auto it = std::find_if(symbol_table.begin(), symbol_table.end(),
                                [&s](const auto& entry) { return entry.first == s.getWord(); });

            if (it == symbol_table.end()) { 
                if (val >= codecount) { 
                    CheckCodeCountOutOfRange(module_num, s, val, codecount);
                    val = 0;
                }
            }

            makeSym(s.getWord(), val + (charPs - codecount), false, false, false, module_num+1);
        }
        module_num++;
    }
    sort(symbol_table.begin(), symbol_table.end(),
              [](const auto& lhs, const auto& rhs) {
                  return lhs.second.getModule() < rhs.second.getModule();
              });
  
    cout<<"Symbol Table" <<endl;
    printSymbolEntry();

   fclose(file);

     
};

int k=0;

void processEInstruction(int updatedAddr, int bAddress, int operand, int count, int module_counter, int opC,list<string> &uselist,set<string>& used_infact ) {

    int value;
  
    if (operand >= uselist.size()) {
        
        value = opC*1000 +bAddress; 
        printf("%0.3d: ", k);
        printf("%0.4d  Error: External operand exceeds length of uselist; treated as relative=0\n", value);

    } else {
        auto it = uselist.begin();


        std::advance(it, operand);
        string sym = *it;
        bool found = false;
        for ( auto& pair : symbol_table) {
            if (pair.first == sym) {
                found = true;
                Symbol s = pair.second;
                pair.second.setInUse(true);
                used_infact.insert(sym);        
                updatedAddr = (opC * 1000) +  s.getLocation();
                printf("%0.3d: ", k);
                printf("%0.4d \n", updatedAddr);

                break;
            }
        }

        if (!found) {
            updatedAddr = (opC * 1000) +  (0);
            used_infact.insert(sym);
            printf("%0.3d: ", k);

            printf("%0.4d Error: %s is not defined; zero used\n", updatedAddr, sym.c_str());

        }
    }
    k++;
}

void processIInstruction(int &updatedAddr){
    printf("%0.3d: ", k);
    printf("%0.4d\n", updatedAddr);
    k++;
}

void processAInstruction(int &updatedAddr, int &opC, int &opR){
    if (opR>= 512){
        opR=0;
        updatedAddr = opC*1000 +opR;
        printf("%0.3d: ", k);
        printf("%0.4d Error: Absolute address exceeds machine size; zero used\n", updatedAddr);
    } else {
        printf("%0.3d: ", k);
        printf("%0.4d\n", updatedAddr);
    }
    k++;
}

void processRInstruction(int &codeSize, int &opR, int &opC, int &bAddress, int &updatedAddr){
    if (opR >= codeSize){
        opR = 0;
        updatedAddr = opC*1000 + bAddress + opR;
        printf("%0.3d: ", k);
        printf("%0.4d Error: Relative address exceeds module size; relative zero used\n", updatedAddr);
    } else{
        updatedAddr = opC*1000 + bAddress + opR;
        printf("%0.3d: ", k);

        printf("%0.4d\n", updatedAddr);
    }
    k++;
}

void processMInstruction(int bAddress, int opR, int opC, int s, int &updatedAddr){
    if (opR<= s-1){

            bAddress=modules[opR].getBaseAddress();
            updatedAddr = opC*1000 + bAddress;
            printf("%0.3d: ", k);
            printf("%0.4d\n", updatedAddr);
    }else{
            bAddress=modules[0].getBaseAddress();
            updatedAddr = opC*1000 + bAddress + 0;
            printf("%0.3d: ", k);
            printf("%0.4d Error: Illegal module operand ; treated as module=0\n", updatedAddr);

    }
    k++;
}

void PassTwo(const char* input_file) {
    FILE *file;
    ExtractTokenClass tokenizer;
    lCounter=0;
    file=tokenizer.openFile(input_file);
    
    int it = 0;
    int module_counter = 0;


    cout<<"Memory Map"<<endl;

    while(!feof(file)){

        list<string> uselist;
        set<string> used_infact;
        
        int definition = readInt(tokenizer);
        if(definition > 0 || definition ==0){
        int p=0;
            while(p<definition){
                string s = readSymbol(tokenizer);
                int v = readInt(tokenizer);
                p++;
            }
        };
        
        int usecount = readInt(tokenizer);
        if (usecount >= 0) {
            int p=0;
            
                while(p<usecount) {
                    string s = readSymbol(tokenizer);
                    uselist.push_back(s);
                p++;
            }
        }

        int codelength = readInt(tokenizer);
        if(codelength >= 0){
            int p=0;
            while(p<codelength){
                string address = readMARIE(tokenizer);

                int  I = readInt(tokenizer);

                int opCode = I / 1000;
                int operand = I % 1000;

                const char* instrType = address.c_str();
                int InstrAddress = I;

                if(opCode <= 9){
                    int codeSize=modules[module_counter].getCodeCount();
                    int bAddress=modules[module_counter].getBaseAddress();
                    switch (instrType[0]) {
                        case 'E':
                            processEInstruction(InstrAddress, bAddress, operand,p, module_counter,opCode, uselist,used_infact);
                            break;
                        case 'I':
                            if (operand >=900){

                                operand =999;
                                printf("%0.3d: ", k);
                                int NewInstr = opCode*1000 + 999;
                                printf("%0.4d Error: Illegal immediate operand; treated as 999\n", NewInstr);
                                k++;
                             } else{
                                processIInstruction(InstrAddress);
                             }
                            break;
                        case 'A':
                            processAInstruction(InstrAddress, opCode, operand);
                            break;
                        case 'R':

                            processRInstruction(codeSize,operand, opCode, bAddress,InstrAddress);
                            break;
                        case 'M':
                            processMInstruction(bAddress, operand, opCode, modules.size(),InstrAddress);
                            break;

                    }
                }
                else  { 
                    InstrAddress = 9999;
                    opCode = InstrAddress / 1000;
                    operand = InstrAddress % 1000;
                        
                    printf("%0.3d: ", k);
                    printf("%0.4d Error: Illegal opcode; treated as 9999\n", InstrAddress);
                    
                       
                    k++;
                }
                
                p++;
            }
        
            int t = 0;
            for (const auto& str : uselist) {
                    if (used_infact.find(str) == used_infact.end()) {
                        cout<<"Warning: Module "<<module_counter<<": uselist["<<t<<"]="<<str.c_str()<<" was not used"<<endl;     
                      
                    }
                        t++; 
            }
          
            module_counter++;

        }
    }
    auto i = symbol_table.begin();
    cout<<endl;
    std::vector<std::string> printedEntries;

     for (const auto& key : K) {
        auto it = std::find_if(symbol_table.begin(), symbol_table.end(),
                               [&key](const std::pair<std::string, Symbol>& entry) {
                                   return entry.first == key;
                               });
        if (it != symbol_table.end()) {
            const auto& p = *it;
            const auto& word = p.first;
            const auto& symbol = p.second;
            if (std::find(printedEntries.begin(), printedEntries.end(), word) == printedEntries.end()) {

                if(!symbol.isInUse()){
                    printf("Warning: Module %d: %s was defined but never used\n", symbol.getModule()-1, word.c_str());

                }
                printedEntries.push_back(word);
            }

           
            }
        }


    fclose(file);
     
};
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    const char* filename = argv[1];
    ifstream inputFile(filename);

    PassOne(filename);
    cout << endl;
    PassTwo(filename);

    return 0;
}
