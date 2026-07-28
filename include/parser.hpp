/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#pragma once
#include <vector>
#include <string>
#include "ast.hpp"
using namespace std;
class Parser {
private:
    unsigned int position = 0;
    vector<Token> tokenize;
public:
	void setTokens(const vector<Token>& tokenize);
	Token peer();
	Token advanced();
	Node* parse_program();
    Node* parse_len();
    Node* parse_typeof();
    Node* parse_include();
	Node* parse_break();
	Node* parse_continue();
    Node* parse_while();
	Node* parse_bool();
    Node* parse_manual();
    Node* parse_if();
	Node* parse_print();
    Node* parse_stod();
	Node* parse_input();
	Node* parse_statement();
	Node* parse_assignment();
	Node* parse_factor();
    Node* parse_boolea_expression();
	Node* parse_logic_expression();
	Node* parse_term();
	Node* parse_expression();
};
