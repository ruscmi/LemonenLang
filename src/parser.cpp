/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include "../include/utf8_win.hpp"
#include <iostream>
void Parser::setTokens(const vector<Token>& tokenize) {
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
	setup_utf8();
	if(!node) { return 0; }
	if(node->KEY == ST_NUMBER) {
		return stod(node->VAL);
	}
	else if(node->KEY == ST_VARIABLE) {
		const string name = node->VAL;
		const auto it = vars.find(name);
		if (it != vars.end()) {
			if(holds_alternative<double>(it->second)) {
				return get<double>(it->second);
			}
			else if(holds_alternative<string>(it->second)) {
				cout<<"\033[33;44mE: var is not str,fucked asshole\033[0m\n"<<endl;				
				return 0;
			}
		}
		return 0;
	}
	else if(node->KEY == ST_OPERATOR) {
		const double left = evaluate(node->left_index);
		const double right = evaluate(node->right_index);
		string op = node->VAL;
		if(op == "+") { int mo = left + right; return mo; }
		else if(op == "-") { int mo = left - right; return mo; }
		else if(op == "/") {
			if(right != 0) {
				int mo = left / right;  return mo; 
			}
			else {
				cout<<"\033[1;31mE: cannot be divided by fucked zero\033[0m"<<endl;
			}
		}				
		else if(op == "*") { int mo = left * right;  return mo;  }
	}
	else if(node->KEY == ST_ASSIGNMENT) {
		string name = node->left_index->VAL;
		if(node->right_index->KEY == ST_NUMBER) {
			vars[name] = stod(node->right_index->VAL);
			//cout<<"сохранил"<<endl;
		}
		else if(node->right_index->KEY == ST_STRING) {
			vars[name] = node->right_index->VAL;
			//cout<<"сохранил"<<endl;
		}
		else if(node->right_index->KEY == ST_VARIABLE) {
			if(vars.find(node->right_index->VAL) != vars.end() ) {
				vars[name] = vars[node->right_index->VAL];
			}else {
				cout<<"\033[1;33mE: variable not found, fuck moron\033[0m"<<endl;
			}
		}else {
			double result = evaluate(node->right_index);
			vars[name] = result;
		}
		return 0;
	}
	else if(node->KEY == ST_PRINT) {
		Node* expr = node->right_index;
		if(expr->KEY == ST_NUMBER) {
			cout<<stod(expr->VAL)<<endl;
		}
		else if(expr->KEY == ST_VARIABLE) {
			string name = expr->VAL;
			const auto it = vars.find(name);
			if(it != vars.end() ) {
				if(holds_alternative<double>(it->second)) {
					cout<<get<double>(it->second)<<endl;
				}else if(holds_alternative<string>(it->second)) {
					cout<<get<string>(it->second)<<endl;
				}
			}else {
				cout<<"\033[1;33mE: var not found: \033[0m"<<name<<endl;
			}
		}
		else if(expr->KEY == ST_STRING) {
			cout<<expr->VAL<<endl;
		}
		else {
			double result = evaluate(expr);
			cout<<result<<endl;
		}
		return 0;
	}
	else {
		cout<<"\033[1;31mE: unknown operator in ST_OPERATOR\033[0m"<<endl;
		return 0;
	}
	return 0;
}
Node* Parser::parse_program() {
	setup_utf8();
	for(const auto& token : tokenize ) {
		if(token.KEY == TTYPE::UNKNOWN ) {
			cout<<"\033[1;31mE: Deer,my parser doesn't understand this shit\033[0m"<<endl;
			return nullptr;
		}
	}
	if(peer().KEY == TTYPE::STRING && tokenize.size() > position + 1 && 
	tokenize[position + 1].KEY == TTYPE::OPERATOR && tokenize[position + 1].VAL == "=" ) {
		return parse_assignment();
	}
	else if(peer().KEY == TTYPE::STRING && peer().VAL == "lmuck") {
		return parse_statement();
	}
	else if(peer().KEY == TTYPE::STRING && peer().VAL == "man") {
		return parse_manual();
	}
	else if(peer().KEY == TTYPE::NUMBER && tokenize.size() > position + 1 && 
	tokenize[position + 1].KEY == TTYPE::OPERATOR && tokenize[position + 1].VAL == "=" ) {
		cout<<"\033[1;31mE: You, asshole, can't assign TTYPE::NUMBER to TTYPE::STRING or TTYPE::UNKNOWN(void)\033[0m"<<endl;
		return nullptr;
	}
	else if(peer().KEY == TTYPE::OPERATOR && peer().VAL == "=" ) {
		cout<<"\033[1;31mE: You fucker, you can't just write 'equals'\033[0m"<<endl;
		return nullptr;
	}
	else {
		return parse_expression();
	}
}
Node* Parser::parse_manual() {
	setup_utf8();
	Token current = peer();
	const char* bluec = "\033[34m";
	const char* resbc = "\033[0m";
	if(current.KEY == TTYPE::STRING && current.VAL == "man") {
		advanced();
		if(peer().KEY == TTYPE::STRING && peer().VAL == "list") {
			cout<<bluec<<R"(   Hello buddy!
		Args for man
 math - see algebraic examples
 assignment - see examples assignments
 print - see examples prints vars or strings)"<<resbc<<endl;
		}
		else if (peer().KEY == TTYPE::STRING && peer().VAL == "math") {
		cout<<bluec<<R"(	What can lmnlang do in math?
 Operations: addition, subtraction, multiplication and division
 Examples: 
 	5 + 5 (will output the result)
 	5 * 5
 	5 / 5
 	5 - 5
 Actions with multiple numbers and operations,examples:
 	5 + 5 * 5 / 5 (will output the result)
 Actions with parentheses (priorities),examples:
 	5 * ( 5 + 5 ) / 5 + 5
 At this time, it's all thanks for viewing this part of the manual.)"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "assignment") {
			cout<<bluec<<R"(	How to use assignment?
 You can create variables as follows:
 	x = 5 
     but no 
      5 = x
 cannot assign strings to numbers
 You can assign whole lines if you put them in quotation marks
  examples: 
  	x = \"hello,world\" 
  	  x = \"500\" (it will be like a string, not a number)
  You can assign a variable to a different variable
   examples:
  	 y = 5
  	  x = y
  when displayed, it will output the values of the previous variable(y)

  You can also store entire algebraic calculations in variables
    examples:
     x = 5 + 5 - 5
      y = 5 * ( 5 + 5 )
  when the variable is displayed, it will output a calculation
  Thank you for reading this part of the manual!)"<<resbc<<endl;
		}
		else if(peer().KEY == TTYPE::STRING && peer().VAL == "print") {
			cout<<bluec<<R"(	I want to say right away! 
 In this language, the 'lmuck' + <args> command (short for lmn druck) is used to output strings
 	How do I output variables?
 Let's say you created a variable y with a value of 5
  example:
  	y = 5
  You can withdraw it by simply writing lmuck y
   example:
   	lmuck y
  How do I output numbers?
   example:
   	lmuck 5 
   	 lmuck 50
   	  lmuck 500
   will give you the numbers.
   How do I print strings?
   	use a quotes for print strings
   	 example:
   	  lmuck \"hello,world\"
   	will bring you 'hello,world'
   It is also possible to output the results of algebraic calculations.
    example:
     lmuck 5 + 5 * 5 / 5
   will bring you answer
   That's all, thank you for viewing this part of the manual.)"<<resbc<<endl;
		}
		else {
			cout<<"\033[1;33mW: use man <arg> and <list> to see args\033[0m"<<endl;
		}
	}
	return nullptr;
}
Node* Parser::parse_print() {
	setup_utf8();
	advanced();
	Node* expr = parse_expression();
	if(expr == nullptr) {
		cout<<"\033[1;33mE: you didn't add the arguments fucked mudda\033[0m"<<endl;
		return nullptr;
	}
	Token current = peer();
	Node* node = new Node();
	node->KEY = ST_PRINT;
	node->VAL = "lmuck";
	node->left_index =  nullptr;
	node->right_index = expr;
	advanced();
	return node;
}
Node* Parser::parse_statement() {
	Token current = peer();
	if (current.KEY == TTYPE::STRING && current.VAL == "lmuck") {
	    return parse_print();
	}
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
	setup_utf8();
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
	if(current.KEY == TTYPE::STRING_LIT) {
		Node* node = new Node();
		node->KEY = ST_STRING;
		node->VAL = current.VAL;
		advanced();
		return node;
	}
	if(current.KEY == TTYPE::SEPARATOR && current.VAL == "\"") {
		advanced();
		Node* inner = parse_expression();
		if(peer().KEY == TTYPE::SEPARATOR && current.VAL == "\"") {
			advanced();
			return inner;
		}
		else {
			cout<<"\033[1;33mE: small dick on the quotes, excepted '\"'\033[0m"<<endl;
			return nullptr;
		}
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
			cout<<"\033[1;33mE: small tits on the brackets, excepted ')'\033[0m"<<endl;
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
