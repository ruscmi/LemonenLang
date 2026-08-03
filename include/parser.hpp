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
	void error(const string& msg);
	unique_ptr<Node> parse_program();
	unique_ptr<Node> parse_wait();
    unique_ptr<Node> parse_func();
    unique_ptr<Node> parse_return();
    unique_ptr<Node> parse_len();
    unique_ptr<Node> parse_typeof();
    unique_ptr<Node> parse_include();
	unique_ptr<Node> parse_break();
	unique_ptr<Node> parse_continue();
    unique_ptr<Node> parse_while();
	unique_ptr<Node> parse_bool();
    unique_ptr<Node> parse_manual();
    unique_ptr<Node> parse_if();
    unique_ptr<Node> parse_print();
    unique_ptr<Node> parse_stod();
    unique_ptr<Node> parse_input();
	unique_ptr<Node> parse_statement();
	unique_ptr<Node> parse_assignment();
	unique_ptr<Node> parse_factor();
	unique_ptr<Node> parse_boolea_expression();
	unique_ptr<Node> parse_logic_expression();
	unique_ptr<Node> parse_term();
	unique_ptr<Node> parse_expression();
};
