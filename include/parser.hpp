/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#pragma once
#include <vector>
#include <string>
#include "ast.hpp"
class Parser {
private:
    unsigned int position = 0;
    std::vector<Token> tokenize;
public:
	void setTokens(const std::vector<Token>& tokenize);
	Token peer();
	Token advanced();
	void error(const std::string& msg);
	std::unique_ptr<Node> parse_program();
	std::unique_ptr<Node> parse_for();
    std::unique_ptr<Node> parse_func();
    std::unique_ptr<Node> parse_return();
    std::unique_ptr<Node> parse_len();
    std::unique_ptr<Node> parse_typeof();
    std::unique_ptr<Node> parse_include();
	std::unique_ptr<Node> parse_break();
	std::unique_ptr<Node> parse_continue();
    std::unique_ptr<Node> parse_while();
	std::unique_ptr<Node> parse_bool();
    std::unique_ptr<Node> parse_manual();
    std::unique_ptr<Node> parse_if();
    std::unique_ptr<Node> parse_print();
    std::unique_ptr<Node> parse_stod();
    std::unique_ptr<Node> parse_input();
	std::unique_ptr<Node> parse_statement();
	std::unique_ptr<Node> parse_assignment();
	std::unique_ptr<Node> parse_factor();
	std::unique_ptr<Node> parse_boolea_expression();
	std::unique_ptr<Node> parse_logic_expression();
	std::unique_ptr<Node> parse_term();
	std::unique_ptr<Node> parse_expression();
};
