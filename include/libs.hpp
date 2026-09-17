#pragma once
#include <iostream>
#include <vector>
#include <optional>
#include "interpreter.hpp"
#include "ast.hpp"
void execute_error(const std::string, Node* node);
std::optional<Value>builtin_exec(const std::string & name,const std::vector<Value>& args,Node* node,bool is_sys);
using LibExecFunc = std::optional<Value> (*)(const std::string&,const std::vector<Value>&,Node*,bool);
std::vector<LibExecFunc>& get_res_libs();
