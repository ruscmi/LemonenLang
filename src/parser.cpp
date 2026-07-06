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
double Parser::evaluate(Node* node) {
	if(!node) { return 0; }
	if(node->KEY == ST_NUMBER) {
		return stod(node->VAL);
	}
	else if(node->KEY == ST_VARIABLE) {
		string name = node->VAL;
		auto it = vars.find(name);
		if (it != vars.end()) {
			return it->second;
		}
	}
	else if(node->KEY == ST_OPERATOR) {
		const double left = evaluate(node->left_index);
		const double right = evaluate(node->right_index);
		string op = node->VAL;
		if(op == "+") { return left + right; }
		else if(op == "-") { return left - right; }
		else if(op == "/") {
			if(right != 0) {
				return left / right; 
			}
			else {
				cout<<"E: cannot be divided by zero"<<endl;
			}
		}				
		else if(op == "*") { return left * right; }
	}
	else if(node->KEY == ST_ASSIGNMENT) {
		string name = node->left_index->VAL;
		double value = evaluate(node->right_index);
		vars[name] = value;
		return value;
	}
	else {
		cout<<"E: unknown operator in ST_OPERATOR"<<endl;
		return 0;
	}
	return 0;
}
Node* Parser::parse_program() {
	for(const auto& token : tokenize ) {
		if(token.KEY == TTYPE::UNKNOWN ) {
			cout<<"E: Deer,my parser doesn't understand this shit "<<endl;
			return nullptr;
		}
	}
	if(peer().KEY == TTYPE::STRING && tokenize.size() > position + 1 && 
	tokenize[position + 1].KEY == TTYPE::OPERATOR && tokenize[position + 1].VAL == "=" ) {
		return parse_assignment();
	}
	else if(peer().KEY == TTYPE::NUMBER && tokenize.size() > position + 1 && 
	tokenize[position + 1].KEY == TTYPE::OPERATOR && tokenize[position + 1].VAL == "=" ) {
		cout<<"E: You, asshole, can't assign TTYPE::NUMBER to TTYPE::STRING or TTYPE::UNKNOWN(void) "<<endl;
		return nullptr;
	}
	else if(peer().KEY == TTYPE::OPERATOR && peer().VAL == "=" ) {
		cout<<"E: You fucker, you can't just write 'equals' "<<endl;
		return nullptr;
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
	const string var_name = current.VAL;
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
			//cout << "DEBUG: Внутри скобок, текущий токен: " << peer().VAL << endl;
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
