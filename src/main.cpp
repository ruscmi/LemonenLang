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
//fuck proprietary
#include "../include/ast.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include "../include/utf8_win.hpp"
#include <readline/readline.h>
#include <readline/history.h>
#include <fstream>
#include <iostream>
extern bool is_runner;
int main(int argc, char *argv[]) {
  setup_utf8();
  const char *big_txt = "\033[1;34m";
  const char *end = "\033[0m";
  if (argc >= 3 && string(argv[1]) == "--file") {
  	is_runner = true;
    Parser pa;
    string filename = argv[2];
    ifstream file(filename);
    unsigned int pos = filename.find_last_of('.');
    if (pos != string::npos) {
      string tstring = filename.substr(pos + 1);
      if (tstring == "lmn") {
        LEX lexing;
        string code((istreambuf_iterator<char>(file)),
                    istreambuf_iterator<char>());
        vector<Token> tokenize = lexing.tokenize(code);
        pa.setTokens(tokenize);
        Node* tree = pa.parse_program();
		if (tree != nullptr && is_runner == true) {
			pa.evaluate(tree);
		}
        file.close();
      } else {
        cout << "\033[1;34mExcepted \'.lmn\' on name file\033[0m" << endl;
      }
    }
  }
  else if (argc >= 2 && string(argv[1]) == "--man") {
     cout<<big_txt<<R"(	How open files in lmnlang?
 type: 
   ./lmnlang --file <file_name>
   WARNING: Only files with the .lmn extension open.)" <<end<<endl;
  }
   else if (argc == 1) {
    setup_utf8();
    bool ActiveRequest = true;
    Parser p;
    LEX lexing;
    cout << big_txt << R"(    lmnlang REPL mode 
  Read Eval Print Loop mode
	 by ruscmi V 0.1
  type 'man list' for manual
	  )"
         << end << endl;
    while (ActiveRequest) {
      char* prompt = readline("#> ");
      bool is_exit = false;
      if(!prompt) {
        cout<<"\033[1;34mGoodbye lemon!\033[0m"<<endl;
        break;
      }
      string inpline(prompt);
      free(prompt);
      if (inpline.empty()) {
        continue;
      }
      add_history(inpline.c_str());
      if (inpline == "exit" || inpline == "quit" ) {
        is_exit = true;
        break;
      }
      if(is_exit) {
         ActiveRequest = false;
         break;
      }
      vector<Token> tokenize = lexing.tokenize(inpline);
      p.setTokens(tokenize);
      Node *tree = p.parse_program();
      // cout<<"==AST-TREE=="<<endl;
      if(tree != nullptr) {
        Value res = p.evaluate(tree);
          if (tree->KEY != ST_ASSIGNMENT &&
          tree->KEY != ST_NOP && tree->VAL != "=") {
              print_array(res);
              cout<<endl;
          }else if(tree->KEY != ST_ASSIGNMENT && tree->KEY != ST_PRINT && tree->KEY != ST_NOP && tree->VAL != "=") {
              cout<<"\033[1;31mE: return TTYPE::UNKNOWN && cannot display this res\033[0m"<<endl;
              return 0;
          }
        delete tree;
      }
    }
    clear_history();
  }else {
    cout << "\033[1;34mE: just not open file\033[0m" << endl;
    return 1;
  }
}
