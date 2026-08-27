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
#include "../include/interpreter.hpp"
#if defined(__linux__) || defined(__APPLE__)
#include <readline/readline.h>
#include <readline/history.h>
#endif
#include <fstream>
#include <iostream>
extern bool is_runner;
using namespace std;
void print_tui_header() {
    cout << "\033[H\033[J";
    cout << "\t   ┌───► REPL [\\]                           \n";
    cout << "\t   │                                         \n";
    cout << "\t   ├───► Manuals [?]                         \n";
    cout << "\t   │                                         \n";
    cout << "\t   ├───► Runner [!]                          \n";
    cout << "\t   │                                         \n";
    cout << "\t   ├───► Exit [-]                            \n";
    cout << "\t   ├───────────────────── inp choise in []   \n";
}
int main(int argc, char *argv[]) {
	interpreter inter;
	const char *big_txt = "\033[1;34m";
	const char *end = "\033[0m";
	auto print_man = [&]() ->void {
		cout<<R"(hello this manual to start manual in lmnlang LOL. 
I was too lazy to refactor the src/manual.cpp file, 
	especially since it's a piece of parser, 
		so here's a manual on how to run the manual and a few more useful things:
	lmnlang man list
execute manual list 
		to run files manually:
	lmnlang /path/to/file
	or on Proprietary Windows:
	.\\build\lmnlang \path\\to\\file
		all support arguments to lmnlang run:
	lmnlang - REPL
	lmnlang tui - TUI menu
	lmnlang /file/to/path - Runner
this is all LOL thx for reading)"<<endl;
	};
	auto file_runner = [&](string prompt) ->void {
		is_runner = true;
	    Parser pa;
	    ifstream file(prompt);
	    unsigned int pos = prompt.find_last_of('.');
	    if (pos != string::npos) {
	      string tstring = prompt.substr(pos + 1);
	      if (tstring == "lmn") {
	        LEX lexing;
	        string code((istreambuf_iterator<char>(file)),
	                    istreambuf_iterator<char>());
	        vector<Token> tokenize = lexing.tokenize(code);
	        pa.setTokens(tokenize);
	        unique_ptr<Node> tree = pa.parse_program();
	//      print_tree(tree,1);
			if (tree != nullptr && is_runner == true) {
				inter.evaluate(tree.get());
			}
	        file.close();
	      } else {
	        cout << "\033[1;34mExcepted \'.lmn\' on name file\033[0m" << endl;
	      }
	  }
  };
   auto repl_mod = [&]() ->void {
  	bool ActiveRequest = true;
  	    cout << big_txt << R"(    lmnlang REPL mode 
  Read Eval Print Loop mode
	 by ruscmi V 0.2
  type 'man list' for manual
  		  )"<< end << endl;
  	    while (ActiveRequest) {
  	      string inpline;
  	      Parser p;
  	      LEX lexing;
  	      #if defined(__linux__) || defined(__APPLE__) 
  	      char* prompt = readline("#> ");
  	      if(!prompt) {
  	        cout<<"\033[1;34mGoodbye lemon!\033[0m"<<endl;
  	        break;
  	      }
  	      inpline = prompt;
  	      free(prompt);
  	      add_history(inpline.c_str());
  	      #elif defined(_WIN32) || defined(_WIN64)
  	      cout<<"#>";
  	      getline(cin,inpline);
  	      #else
  	      #error "unknown platform"
  	      #endif
  	      if (inpline.empty()) {
  	        continue;
  	      }
  	      if (inpline == "exit" || inpline == "quit" ) {
  	        break;
  	      }
  	      vector<Token> tokenize = lexing.tokenize(inpline);
  	      p.setTokens(tokenize);
  	      unique_ptr<Node>tree = p.parse_program();
  	//    print_tree(tree.get(),1);
  	      if(tree != nullptr) {
  	        Value res = inter.evaluate(tree.get());
  	        if(!holds_alternative<AcceptValue>(res)) {
  	            print_array(res);
  	            cout<<endl;
  	        }
  	      }
  	    }
  	    #if defined(__linux__) || defined(__APPLE__)
  	    clear_history();
  	    #endif
  };
  if (argc == 2 && string_view(argv[1]) == "tui") {
  	#if defined(__linux__) || defined(__APPLE__)
  	using_history();
  	#endif
  	string prompt = 
  		"\t   │ \n"
  		"\t   └───────► ";
  	string inp;
	while(true) {
		print_tui_header(); 
		#if defined(__linux__) || defined(__APPLE__)
 		char* input = readline(prompt.c_str());
 		if(!input) {
 			cout<<"\nGoodBye Lemon!"<<endl;
 			break;
 		}
 		inp = input;
 		free(input);
 		if(!inp.empty()) {
 			add_history(inp.c_str());
 		}
 		#else
 		cout<<prompt;
 		if(cin.fail()) {
 			cin.clear();
 		}
 		getline(cin,inp);
 		#endif
 		if(inp == "-") {
 			break;
   		}
   		else if(inp == "!") {
   			cout<<"input a file path or file name: "<<endl;
   			string filepath;
   			cin >> ws;
   			getline(cin,filepath);
   			file_runner(filepath);
   			break;
   		}
   		
   		else if(inp == "\\") {
   			repl_mod();
   			break;
   		}
   		else if(inp == "?") {
   			print_man();
   			cout<<"inp any хуйню to exit: "<<endl;
   			string x;
   			cin>>x;
   			cin.ignore(1000,'\n');
   		}
   		else {
   			cout<<"unknown value"<<endl;
   		}
  	}
  }
  else if (argc == 2) {
  	string suc_string = argv[1];
  	file_runner(suc_string);
  }
   else if (argc == 1) {
    repl_mod();
  }else {
    cout << "\033[1;34mE: just not open file\033[0m" << endl;
    return 1;
  }
}
