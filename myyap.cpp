//LemonLang source code
//thx for reading the source btw
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <fstream> //import libs
#include <thread>
#include <chrono>
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
        if(v.empty()) continue;
        if(active==false && v[0] != "ansonsten" && v[0] != "ende") {
			continue;
		}
        if(v[0] == "quit") {
            isRunning = false; //quit command
            break;
        }
        else if(v[0] =="wenn") {
			if(v.size() ==4) {
			    std::string v1;
			    std::string v3;
				if(m.count(v[1]) > 0) {	//wenn(If) command
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
printl - Variable output or input string(print hello world)
set - create a variable (set x 10)
math - Easy math (math res 5 + 5)
quit  - Quit in ezcmi
run - Opening files with .cmi extension(run abc) 
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
       else if(v[0] == "run") { //the same logic here, only for files that are opened
            if(v.size() == 2) {
                std::ifstream file(v[1]);
                if(file.is_open()) {
                    std::string line;
                    while (std::getline(file, line)) {
                        std::stringstream ss(line);
                        std::vector<std::string> v_file;
                        std::string wd;
                        while(ss >> wd) {
                            v_file.push_back(wd);
                        }
                        if(!v_file.empty()) {
						if(active==false && v_file[0] != "ansonsten.log" && v_file[0] != "ende.log") {
			                continue;
		                }
                            if(v_file[0] == "set") {
                                if(v_file.size() >= 3) {
                                    m[v_file[1]] = v_file[2];
                                    std::cout << "Saved: " << v_file[1] << " = " << m[v_file[1]] << std::endl;
                                }
                            }
                           else if(v_file[0] =="wenn") {
								if(v_file.size() ==4) {
									std::string v1;
									std::string v3;
									if(m.count(v_file[1]) > 0) {	
										v1 = m[v_file[1]];
									}else{
										v1 = v_file[1];
									}
									std::string v2 = v_file[2];
									if(m.count(v_file[3]) > 0) {
										v3 = m[v_file[3]];
									}else{
										v3 = v_file[3];
									}
									try {
										double lw = std::stod(v1);
										double rw = std::stod(v3);
										if(v2 == "==") {
											if(lw == rw) {
												active = true;
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
							else if(v_file[0] == "ende") {
								active = true;
							}
							else if(v_file[0] == "ansonsten") {
								if(met == false) {
									active = true;
								}else{
									active = false;
								}
							}
                            else if(v_file[0]=="textinp") {
								if(v_file.size()>=2) {
									std::string nm = v_file[1];
									std::string vm;
									std::cout<<"inp"<<nm<<": ";
									std::getline(std::cin >>std::ws,vm);
									m[v_file[1]] = vm;
								}
							}
                            else if(v_file[0] == "printl") {
                                for(size_t i = 1; i < v_file.size(); i++) {
                                    if(m.count(v_file[i]) > 0) std::cout << m[v_file[i]] << " ";
                                    else std::cout << v_file[i] << " ";
                                }
                                std::cout << std::endl;
                            }
                            else if(v_file[0]=="wait") {
								if(v_file.size()==2) {
									int nt = std::stoi(v_file[1]);
									std::this_thread::sleep_for(std::chrono::milliseconds(nt));
								}
							}
                            else if(v_file[0] == "math") {
                                if(v_file.size() == 5) {
                                    try {
                                        double n1 = (m.count(v_file[2])) ? std::stod(m[v_file[2]]) : std::stod(v_file[2]);
                                        double n2 = (m.count(v_file[4])) ? std::stod(m[v_file[4]]) : std::stod(v_file[4]);
                                        char op = v_file[3][0];
                                        double res = 0;
                                        switch(op) {
                                            case '+': res = n1 + n2; break;
                                            case '-': res = n1 - n2; break;
                                            case '*': res = n1 * n2; break;
                                            case '/': if(n2 != 0) res = n1 / n2; break;
                                        }
                                        m[v_file[1]] = std::to_string(res);
                                        std::cout << res << std::endl;
                                    } catch(...) {
                                        std::cout << "Error: ValueError in file" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                    file.close();
					active = true;
					met = true;
					std::cin.clear();
                } else {
                    std::cout << "Not found" << std::endl;
                }
            }
        }
        else {
            std::cout<<"E: Command '"<<v[0]<<"' not found"<<std::endl; 
        }
    }
}
//to be continued...
