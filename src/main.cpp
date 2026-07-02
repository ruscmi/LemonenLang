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
int main() {
  LEX lexing;
  string code;
  while (ActiveRequest) {
    cout << "#> ";
    getline(cin, code);
    if(code.empty()) { continue; }
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
