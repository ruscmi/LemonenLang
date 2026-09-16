#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#if defined(__linux__) || defined(__APPLE__)
#include <readline/readline.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#elif defined(__linux__)
#include <linux/input.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__ANDROID__)
#include <ctime>
#endif
#include <thread>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <optional>
#include "interpreter.hpp"
#include "ast.hpp"
void execute_error(const std::string, Node* node);
std::optional<Value>builtin_exec(const std::string & name,const std::vector<Value>& args,Node* node,bool is_sys);
