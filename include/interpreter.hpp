#pragma once
#include <unordered_map>
#include <string>
#include "ast.hpp"
#include <unordered_set>
class interpreter {
private:
    vector<unordered_map<string,Value>> vars;
    bool is_sys = false;
public:
    void execute_error(const string& msg,Node* node);
    interpreter() {
        vars.push_back({});
    }
    Value evaluate(Node* node);
};
