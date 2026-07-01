/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include "ast.hpp"
using namespace std;
class Parser {
private:
    unsigned int position = 0;
    vector<Token> tokenize;
    unordered_map<string,double> vars;
public:
	Parser(vector<Token> tokenize);
	Token peer();
	Token advanced();
	Node* parse_program();
	Node* parse_factor();
	Node* parse_term();
	Node* parse_expression();
};
