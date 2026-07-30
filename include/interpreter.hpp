#pragma once
#include <unordered_map>
#include <string>
#include "ast.hpp"
class interpreter {
private:
    vector<unordered_map<string,Value>> vars;
public:
    interpreter() {
        vars.push_back({});
    }
    Value evaluate(Node* node);
};
