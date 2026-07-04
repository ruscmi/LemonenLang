/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include <iostream>
Parser::Parser(std::vector<Token>tokenize) {
	this->tokenize = tokenize;
	this->position = 0;
}
Token Parser::peer() {
	if(position >= tokenize.size()) {
		return Token{TTYPE::END,""};
	}
	return tokenize[position];
}
Token Parser::advanced() {
	if(position < tokenize.size()) {
		position++; 
	}
	return tokenize[position -1];
}
Node* Parser::parse_program() {
	if(peer().KEY == TTYPE::STRING && tokenize[position + 1].KEY == TTYPE::OPERATOR && tokenize[position + 1].VAL == "=" ) {
		return parse_assignment();
	}
	else {
		return parse_expression();
	}
}
Node* Parser::parse_statement() {
	Token current = peer();
	if(current.KEY == TTYPE::STRING) {
		if(tokenize[position+1].KEY == TTYPE::OPERATOR && tokenize[position + 1].VAL == "=") {	
			return parse_assignment();
		}	
		else {
			Node* node = new Node();
			node->KEY = ST_VARIABLE;
			node->VAL = current.VAL;
			advanced();
			return node;
   		}
	}
	return parse_expression();
}
Node* Parser::parse_assignment() {
	Token current = peer();
	string var_name = current.VAL;
	advanced(); 
	if(peer().KEY == TTYPE::OPERATOR && peer().VAL == "=") {
		advanced();
		Node* right_expr = parse_expression();
		Node* node = new Node();
		node->KEY = ST_VARIABLE;
		node->VAL = var_name;

		Node* assign = new Node();
		assign->KEY = ST_ASSIGNMENT;
		assign->VAL = "=";
		assign->left_index = node;
		assign->right_index = right_expr;
		return assign;
	}
	return nullptr;
}
Node* Parser::parse_factor() {
	Token current = peer();
	// cout << "DEBUG: TOKEN " << current.VAL << ", KEY = " << current.KEY << endl;
	if(current.KEY == TTYPE::NUMBER) {
		Node* node = new Node();
		node->KEY = ST_NUMBER;
		node->VAL = current.VAL;
		advanced();
		return node;
	}
	if(current.KEY == TTYPE::STRING) {
		Node* node = new Node();
		node->KEY = ST_VARIABLE;
		node->VAL = current.VAL;
		advanced();
		return node;
	}
	if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(" ) {
		advanced();
		Node* inner = parse_expression();
		// cout << "DEBUG: TOKEN: " << peer().VAL << endl;
		if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
			advanced();
			return inner;
			cout << "DEBUG: Внутри скобок, текущий токен: " << peer().VAL << endl;
		}
		else {
			cout<<"E: small tits on the brackets, excepted ')' "<<endl;
			return nullptr;
		}
	}
	else {
		return nullptr;
	}
}
Node* Parser::parse_term() {
	Node* left = parse_factor();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR && (current.VAL == "*" || current.VAL == "/")) {
		const string current_op = current.VAL;
		advanced();
		Node* right = parse_factor();
		if(right == nullptr) {
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;
		current = peer();
	}
	return left;
}
Node* Parser::parse_expression() {
	Node* left = parse_term();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR &&  (current.VAL == "+" || current.VAL == "-")) {
		const string current_op = current.VAL;
		advanced();
		Node* right = parse_term();
		if(right == nullptr) {
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;
		current = peer();
	}
	return left;
}
