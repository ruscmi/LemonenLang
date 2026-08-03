#pragma once
#include <unordered_map>
#include <string>
#include "ast.hpp"
#include <unordered_set>
class interpreter {
private:
    std::vector<std::unordered_map<std::string,Value>> vars;
    bool is_sys = false;
public:
    void execute_error(const std::string& msg,Node* node);
    interpreter() {
        vars.push_back({});
    }
    Value evaluate(Node* node);
};
