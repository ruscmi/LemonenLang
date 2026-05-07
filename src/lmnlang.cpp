#include "logic.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <fstream>

void executeCommands(std::vector<std::string>& v,std::map<std::string,std::string>& m,bool& active,bool& met,bool& isRunning) {
    if(v.empty()) return;
    
    if(active==false && v[0] != "ansonsten" && v[0] != "ende") {
        return;
    }
    if(v[0] == "quit") {
        isRunning = false; //quit command
    } 
            else if(v[0] =="wenn") {
                        if(v.size() ==4) {
                            std::string v1;
                            std::string v3;
                                if(m.count(v[1]) > 0) { //wenn(If) command
                                    v1 = m[v[1]];
                                }else{
                                        v1 = v[1];
                                }
                                std::string v2 = v[2];
                                if(m.count(v[3]) > 0) {
                                    v3 = m[v[3]];
                            }else{
                                        v3 = v[3];
                                }
                                try {
                                        double lw = std::stod(v1);
                                        double rw = std::stod(v3);
                                        if(v2 == "==") {
                                                if(lw == rw) {
                                                        active = true; //this is used to compare numbers or variables
                                                        met = true;
                                                }else{
                                                        active = false;
                                                        met = false;
                                                }
                                         }
                                        else if(v2 ==">") {
                                                if(lw > rw) {
                                                        active = true;
                                                        met = true;
                                                }else{
                                                        active = false;
                                                        met = false;
                                                }
                                        }
                                        else if(v2=="<") {
                                                if(lw<rw) {
                                                        active = true;
                                                        met = true;
                                                }else{
                                                        active = false;
                                                        met = false;
                                                }
                                        }
                                }
                                catch (...) {
                                        if(v2 == "==") {
                                                active = (v1 == v3);
                                                met = active;
                                        }else {
                                                active = false;
                                                met = false;
                                        }
                                }
                        }
            }
                else if(v[0] == "ende") {
                        active = true;
                }
                else if(v[0] == "ansonsten") {
                        if(met == false) {
                            active = true;
                        }else{
                                active = false;
                        }
                }
        else if(v[0] == "set") {
            if(v.size() >= 3) {
                m[v[1]] = v[2];
                std::cout<<"Saved to memory: "<<v[1]<<" = "<<m[v[1]]<<std::endl;
            }
        }
        else if(v[0] == "printl") {
            for(size_t i = 1; i < v.size(); i++) {
                if(m.count(v[i]) > 0) {
                    std::cout<<m[v[i]]<<" ";
                } else {
                    std::cout<<v[i]<<" "; //output of variables or output of made-up sentences by you
                }
            }
            std::cout<<std::endl;
        }
        else if(v[0] == "--help") {
            std::string help = R"(Commands:
printl - Variable output or input string(printl hello world)
set - create a variable (set x 10)
math - Easy math (math res 5 + 5)
quit  - Quit in lmnlang
run - Opening files with .lmn extension(run abc)
textinp - Input to string
wenn - value if
ansonsten - value else
ende - meaning of the end of logic  )"; //i added this to make the language easier, I'm trying to make it easier for children
            std::cout<<help<<std::endl;
        }
        else if(v[0]=="wait") {
                        if(v.size()==2) {
                                int nt = std::stoi(v[1]);
                                std::this_thread::sleep_for(std::chrono::milliseconds(nt));
                        }
                }
        else if(v[0]=="textinp") {
                        if(v.size()>=2) {
                                std::string nm = v[1];
                                std::string vm;
                                std::cout<<"inp "<<nm<<": ";
                                std::getline(std::cin >>std::ws,vm); //input text
                                m[v[1]] = vm;
                    }
                }
            else if(v[0] == "math") {
            if(v.size() == 5) {
                                try {
                                        double n1 = (m.count(v[2])) ? std::stod(m[v[2]]) : std::stod(v[2]);
                                        double n2 = (m.count(v[4])) ? std::stod(m[v[4]]) : std::stod(v[4]);
                                    char op = v[3][0];
                    double res = 0;
                    switch(op) {
                                                case '+': res = n1 + n2; break; //basic math working on a switch to simplify the code
                                                case '-': res = n1 - n2; break;
                                                case '*': res = n1 * n2; break;
                                                case '/': if(n2 != 0) res = n1 / n2; break;
                                        }
                    m[v[1]] = std::to_string(res);
                    std::cout<<res<<std::endl;
                            }
                            catch(const std::exception& e) {
                                        std::cout<<"Error: "<<" ValueError in variables"<<std::endl;
                                }
            }
        }
    else if(v[0] == "run") {
    if(v.size() == 2) {
        std::ifstream file(v[1]); 
        if(file.is_open()) {
            std::string line;
            while (std::getline(file, line)) { 
                std::stringstream ss_file(line);
                std::vector<std::string> v_file;
                std::string word;
                
                while(ss_file >> word) { 
                    v_file.push_back(word);
                }
                executeCommands(v_file, m, active, met, isRunning); 
            }
            file.close();
        } else {
            std::cout << "Error: Could not open file " << v[1] << std::endl;
        }
    }
}   
}
