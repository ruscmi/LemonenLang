/*
lmnlang - lightweight and fast interpreter
Copyright (C) 2026 ruscmi
lmnlang is probided under:
GNU GENERAL PUBLIC LICENSE
                       Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.
 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.

  This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

  This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

  You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
/* 
im love Richard Stollman this code
released under the GPL 2.0 license
FUCK MICROSLOP   🤡  🤡  🤡  🤡  🤡  🤡  🤡   🤡   🤡
*/
#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
bool ActiveRequest = true;
using namespace std;
#pragma region
enum TTYPE { KEYWORD, OPERATOR, SEPARATOR, NUMBER, STRING, UNKNOWN,END};
struct Token {
  TTYPE KEY;
  string VAL;
};
enum ASTTAB { ST_ASSIGNMNET, ST_NUMBER , ST_OPERATOR, ST_VARIABLE, ST_SEPARATOR };
struct Node {
  ASTTAB KEY;
  string VAL;
  Node* left_index;
  Node* right_index;
};
/* == LEXING == */
class LEX {
private:
  vector<Token> tokens;
  Token T;
public:
  vector<Token>& tokenize(const string &code) {
    tokens.clear();
    unsigned int i = 0;
    unsigned int len = code.length();
    while (i < len) {
      unsigned char current = code[i];
      if (isspace(current)) {
        i++;
        continue;
      }
      if (isdigit(current)) {
        string val = "";
        while (i < len && isdigit(code[i])) {
          val += code[i];
          i++;
        }
        T.KEY = TTYPE::NUMBER;
        T.VAL = val;
        tokens.push_back(T);
        continue;
      }
      if (isalpha(current)) {
        string val = "";
        while (i < len && isalpha(code[i])) {
          val += code[i];
          i++;
        }
        T.KEY = TTYPE::STRING;
        T.VAL = val;
        tokens.push_back(T);
        continue;
      }
      if (current == '+' || current == '-' || current == '*' ||
          current == '/' || current == '=' || current == '<' ||
          current == '>') {
        string val(1, current);
        T.KEY = TTYPE::OPERATOR;
        T.VAL = val;
        tokens.push_back(T);
        i++;
        continue;
      }
      if (current == '.' || current == '{' || current == '}' ||
          current == '(' || current == ')') {
        string val(2, current);
        T.KEY = TTYPE::SEPARATOR;
        T.VAL = val;
        tokens.push_back(T);
        i++;
        continue;
      }
      else {
      	string val(1,current);
      	T.KEY = TTYPE::UNKNOWN;
      	T.VAL = val;
      	tokens.push_back(T);
      	cout<<"E: Unknown symbols\n";
      	i++; continue;
      }
    }
    return tokens;
  }
/*  void vector_parsing() {
    for (const auto &token : tokens) {
      cout << token.KEY << " : " << token.VAL << " ";
    }
    cout << endl;
  } */
  // this for debugs
};

/*
  == PARSER ==
 im using AST-tree
 for parsing 
*/
class Parser {
private:
    unsigned int position = 0;
    vector<Token> tokenize;
    unordered_map<string,double> vars;
public:
	Parser(vector<Token> tokenize){
		this->tokenize=tokenize;
	}
	Token peer() {
		if(position >= tokenize.size()) {
			return Token{TTYPE::END,""};
		}
		return tokenize[position];
	}
	Token advanced() {
		if(position < tokenize.size()) {
			position++; 
		}
		return tokenize[position -1];
	}
	Node* parse_program() {
		return parse_expression();
	}
	Node* parse_factor() {
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
	Node* parse_term() {
		Node* left = parse_factor();
		if(left == nullptr) {
			return nullptr;
		}
		Token current = peer();
		while(current.KEY == TTYPE::OPERATOR && (current.VAL == "*" || current.VAL == "/")) {
			string current_op = current.VAL;
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
	Node* parse_expression() {
		Node* left = parse_term();
		if(left == nullptr) {
			return nullptr;
		}
		Token current = peer();
		while(current.KEY == TTYPE::OPERATOR && (current.VAL == "+" || current.VAL == "-")) {
			string current_op = current.VAL;
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
};
void print_tree(Node* node,int level = 0) {
	if(node == nullptr) { return; }
	for(int i = 0; i < level; i++ ) { cout<<"   "; }
	cout<<node->VAL;
	if(node->KEY == ST_NUMBER ) { cout<<" [NUMBER]\n"; }
	else if(node->KEY == ST_OPERATOR) { cout<<" [OPERATOR]\n"; }
	else if(node->KEY == ST_VARIABLE) { cout<<" [VARIABLE]\n"; }
	else { cout<<" [UNKNOWN]"; }
	print_tree(node->left_index, level + 1);
	print_tree(node->right_index, level + 1);
}
int main() {
  LEX lexing;
  string code;
  while (ActiveRequest) {
    cout << "#> ";
    getline(cin, code);
    vector<Token>tokenize = lexing.tokenize(code);
    Parser p(tokenize);
    Node* tree = p.parse_program();
    cout<<"==AST-TREE=="<<endl;
    if(tree != nullptr) {
    	print_tree(tree, 0);
    } else {
    	cout<<"E: nullptr TRUE";
    }
    cout<<endl;
    cout<<"TOKENS: "<<tokenize.size()<<endl;
    if (code == "exit") {
      ActiveRequest = false;
      break;
    }
  }
}
#pragma endregion 
/*      FUCK    VSCODE    FUCK    FUCK    FUCK    👆     👆     👆  */
