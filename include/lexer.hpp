#pragma once
#include <vector>
#include <string>
#include "ast.hpp"
using namespace std;
class LEX {
private:
  vector<Token> tokens;
  Token T;
public:
  vector<Token>& tokenize(const string &code);
};
  
