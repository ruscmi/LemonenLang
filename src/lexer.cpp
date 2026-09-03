/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/lexer.hpp"
#include <cctype>
#include <iostream>
using namespace std;
size_t LEX::get_utf8(unsigned char c) {
    if((c & 0x80) == 0) return 1;
    if((c & 0xE0) == 0xC0) return 2;
    if((c & 0xF0) == 0xE0) return 3;
    if((c & 0xF8) == 0xF0) return 4;
    return 1;
}
vector<Token>& LEX::tokenize(const string &code) {
	tokens.clear();
	unsigned int i = 0;
	unsigned int len = code.length();
	size_t current_line = 1;
    size_t current_col = 1;
	while (i < len) {
	  size_t start_line = current_line;
	  size_t start_col = current_col;
	  unsigned char current = code[i];
	  size_t char_len = get_utf8(current);
	  auto step = [&]() {
	    if(code[i] == '\n') {
	        current_line++;
	        current_col = 1;
	    }else {
	        current_col++;
	    }
	    i++;
	  };
	  if (isspace(current)) {
	    step();
	    continue;
	  }
	  if (isdigit(current)) {
	    string val = "";
	    while (i < len && isdigit(code[i])) {
	      val += code[i];
	      step();
	      if(code[i] == '.') {
	      	val += code[i];
	      	step();
	      }
	    }
	    T.KEY = TTYPE::NUMBER;
	    T.VAL = val;
	    T.LINE = start_line;
	    T.COL = start_col;
	    tokens.push_back(T);
	    continue;
	  }
	  if (current == '$' || current == '@') {
	  	string val(1,current);
	  	T.KEY = TTYPE::SPECSYMB;
	  	T.VAL = val; 
	  	T.LINE = start_line;
	  	T.COL = start_col;
	  	tokens.push_back(T);
	  	step();
	  	continue;
	  }
	  if (char_len > 1 || isalpha(current) || current == '_') {
        string val = "";
        while(i < len) {
            size_t curr = get_utf8(code[i]);
            if(curr > 1) {
                for(size_t k = 0; k < curr; ++k) {
                    val += code[i];
                    step();
                }
            }
            else if (i < len && (isdigit(code[i]) || isalpha(code[i]) 
            || code[i] == '_')) {
                val += code[i];
                step();
            }
            else {
                break;
            }
        }
        T.KEY = TTYPE::STRING;
        T.VAL = val;
        T.LINE = start_line;
        T.COL = start_col;
        tokens.push_back(T);
        continue;
	  }
	  if (current == '/' && i + 1 < len && code[i + 1] == '/') {
	    while(i < len && code[i] != '\n' ) { step(); }
	    continue;
	  }
	  if(i == 0 && current == '#' && i + 1 < len && code[i + 1] == '!') {
	    while(i < len && code[i] != '\n') { step(); }
	    continue;
	  }
	  if(current == '/' && i + 1 < len && code[i + 1] == '-') {
	    while(i + 1 < len && !(code [i] == '-' && code [i + 1] == '/')) { step(); }
	    if(i + 1 < len) {
	        step(); step();
	    }else {
	        cout<<"\033[1;31mE: unclosed note\033[0m"<<endl;
	    }
        continue;
	  }
	  if (current == '>' || current == '<' || 
	  (current == '!' && code[i+1] == '=') || (current == '=' && i + 1 < len && code[i+1] == '=')) {
	    string val = "";
	    val += current;
	    if(i + 1 < len && code[i+1] == '=') {
            val += code[i+1];
            step();
	    }
	    T.KEY = TTYPE::LOGIC_OPERATOR;
	    T.VAL = val;
	    T.LINE = start_line;
	    T.COL = start_col;
	    tokens.push_back(T);
	    step();
	    continue;
	  }
	  if(current == '!') {
	    string val(1,current);
	    T.KEY = TTYPE::NOT;
	    T.VAL = val;
	    T.LINE = start_line;
	    T.COL = start_col;
	    tokens.push_back(T);
	    step();
	    continue;
	  }
	  if((current == '&' && i + 1 < len && code[i+1] == '&') || (current == '|' && i + 1 < len && code[i+1] == '|')) {
	    string val = {code[i],code[i+1]};
	    T.KEY = TTYPE::BOOLEA_OPERATOR;
	    T.VAL = val;
	    T.LINE = start_line;
	    T.COL = start_col;
	    tokens.push_back(T);
	    step(); step();
	    continue;
	  }
	  if (current == '+' || current == '-' || current == '*' ||
	      current == '/' || current == '=' || current == '%') {
	    string val(1, current);
	    T.KEY = TTYPE::OPERATOR;
	    T.VAL = val;
	    T.LINE = start_line;
	    T.COL = start_col;
	    tokens.push_back(T);
	    step();
	    continue;
	  }
	  if(current == ';' ) {
	  	string val(1,current);
	  	T.KEY = TTYPE::END_EX;
	  	T.VAL = val;
	    T.LINE = start_line;
	    T.COL = start_col;
	  	tokens.push_back(T);
	  	step();
	  	continue;
	  }
	  if (current == '[' || current == ']' ||
	      current == '(' || current == ')' || 
	      current == ',' || current == '{' ||
	      current == '}' || current == '.' ||
	      current == ':') {
	    string val(1, current);
	    T.KEY = TTYPE::SEPARATOR;
	    T.VAL = val;
	    T.LINE = start_line;
	    T.COL = start_col;
	    tokens.push_back(T);
	    step();
	    continue;
	  }
	  if(current == '"' || current == '\'' ) {
	    char quote = current;
	  	step();
	  	string val = "";
	  	while(i < len && code[i] != quote) {
	  	    if(code[i] == '\\' && i + 1 < len) {
	  	        if(code[i + 1] == 'n') {
	  	            val += '\n';
	  	        }else if(code[i + 1] == 't') {
	  	            val += '\t';
	  	        }else if(code[i + 1] == 'r') {
	  	            val += '\r';
                }else if(code[i + 1] == '\\') {
	  	            val += '\\';
	  	        }else if(code[i + 1] == 'x') {
                    if(i + 3 < len && isxdigit(code[i + 2]) && isxdigit(code[i + 3])) {
                        string hex = code.substr(i + 2,2);
                        char byte = static_cast<char>(stoi(hex,nullptr,16));
                        val += byte;
                        step(); step();
                    }else {
                        cout<<"\033[1;31mE: invalid hex arguments \033[0m"<<endl;
                        tokens.clear();
                        return tokens;
                    }
	  	        }else if(code[i + 1] == quote) {
	  	            val += quote;
	  	        }else if(code[i + 1] == 'e') {
	  	            val += '\x1B';
	  	        }else if(code[i + 1] == '0'){
	  	            if(i + 3 < len && code[i + 2] == '3' && code[i + 3] == '3') {
	  	                val += '\033';
	  	                step(); step();
	  	            }else {
	  	                val += '\0';
	  	            }
	  	        }else {
	  	            val += '\\';
	  	            val += code[i + 1];
	  	        }
	  	        step(); step();
	  	        continue;
	  	    }
	  		val += code[i];
	  		step();
	  	}
	  	if(i < len && code[i] == quote) {
		  	step();
		  	T.KEY = TTYPE::STRING_LIT;
		  	T.VAL = val;
		  	T.LINE = start_line;
		  	T.COL = start_col;
		  	tokens.push_back(T);
		}
		else {
			cout<<"\033[1;31mE: expected (\033[0m"<<quote<<"\033[1;31m)\033[0m"<<endl;
			tokens.clear();
			return tokens;
		}
	  continue;
	  }
	  else {
	  	string val(1,current);
	  	T.KEY = TTYPE::UNKNOWN;
	  	T.VAL = val;
	  	T.LINE = start_line;
	  	T.COL = start_col;
	  	tokens.push_back(T);
	  	step(); continue;
	  }
  }
  return tokens;
}
