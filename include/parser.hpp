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
    unordered_map<string,Value> vars;
public:
	void setTokens(const vector<Token>& tokenize);
	Token peer();
	Token advanced();
	Value evaluate(Node* node);
	Node* parse_program();
	Node* parse_manual();
	Node* parse_print();
	Node* parse_statement();
	Node* parse_assignment();
	Node* parse_factor();
	Node* parse_term();
	Node* parse_expression();
};
