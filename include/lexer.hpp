/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#pragma once
#include <vector>
#include <string>
#include "ast.hpp"
class LEX {
private:
    std::vector<Token> tokens;
    Token T;
public:
    std::vector<Token>& tokenize(const std::string &code);
    size_t get_utf8(unsigned char c);
};
  
