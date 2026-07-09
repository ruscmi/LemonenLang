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
	FUCK PROPRIETARY SOFT
*/
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/ast.hpp"
#include <iostream>
bool ActiveRequest = true;
Parser p;
int main() {
  LEX lexing;
  string code;
  while (ActiveRequest) {
    cout << "#> ";
    getline(cin, code);
    if(code.empty()) { continue; }
    vector<Token>tokenize = lexing.tokenize(code);
    p.setTokens(tokenize);
    Node* tree = p.parse_program();
    //cout<<"==AST-TREE=="<<endl;
	double res = p.evaluate(tree);
	if(tree != nullptr && tree->KEY != ST_ASSIGNMENT && tree->KEY == ST_OPERATOR && tree->VAL != "=") {
		cout<<res<<endl;
	}
    //cout<<"TOKENS: "<<tokenize.size()<<endl;
    vector<string>exits = {"exit","Exit","EXit","EXIt","EXIT","eXIT","exIT","exiT","eXit","exIt","EXiT","ExIT","ExiT","ExIt"};
	for(const string& exit : exits) {
	    if (code == exit ) {
	      ActiveRequest = false;
	      break;
	    }
	}
  }
}
