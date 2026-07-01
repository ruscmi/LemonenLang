#include "/home/smirnow/lmnlang_v2/include/parser.hpp"
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
	return parse_expression();
}
Node* Parser::parse_factor() {
	Token current = peer();
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
	else {
		cout<<"ERROR PARSING: UNKNOWN AST INDEX"<<endl;	
		advanced();
		return nullptr;
	}
	return nullptr;
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
	while(current.KEY == TTYPE::OPERATOR && (current.VAL == "+" || current.VAL == "-")) {
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
