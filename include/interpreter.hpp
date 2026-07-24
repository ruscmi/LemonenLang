#pragma once
#include <unordered_map>
#include <string>
#include "ast.hpp"
class interpreter {
private:
    unordered_map<string,Value> vars;
public:
    Value evaluate(Node* node);
};
