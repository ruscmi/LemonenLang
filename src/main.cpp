#include "logic.h"
#include <iostream>
#include <string>
#include <sstream>

int main() {
    std::map<std::string, std::string> m;
    bool isRunning = true;
    bool active = true; //here I create flags
    bool met = true;
    while(isRunning) {
        std::vector<std::string> v;
        std::string inpline;
        std::cout<<"input or '--help' for commands info: ";
        std::getline(std::cin, inpline);
        std::stringstream ss(inpline);
        v.clear();
        std::string w;  //here is all the basic logic, creating a vector, memory, parsing strings
        while(ss>>w) {
            v.push_back(w);
        }
        executeCommands(v,m,active,met,isRunning);
    }
    return 0;
}
