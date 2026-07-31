/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/lexer.hpp"
#include "../include/utf8_win.hpp"
#include <cctype>
#include <iostream>
size_t LEX::get_utf8(unsigned char c) {
    if((c & 0x80) == 0) return 1;
    if((c & 0xE0) == 0xC0) return 2;
    if((c & 0xF0) == 0xE0) return 3;
    if((c & 0xF8) == 0xF0) return 4;
    return 1;
}
vector<Token>& LEX::tokenize(const string &code) {
	setup_utf8();
	tokens.clear();
	unsigned int i = 0;
	unsigned int len = code.length();
	while (i < len) {
	  unsigned char current = code[i];
	  size_t char_len = get_utf8(current);
	  if (isspace(current)) {
	    i++;
	    continue;
	  }
	  if (isdigit(current)) {
	    string val = "";
	    while (i < len && isdigit(code[i])) {
	      val += code[i];
	      i++;
	      if(code[i] == '.') {
	      	val += code[i];
	      	i++;
	      }
	    }
	    T.KEY = TTYPE::NUMBER;
	    T.VAL = val;
	    tokens.push_back(T);
	    continue;
	  }
	  if (char_len > 1 || isalpha(current) || current == '_') {
        string val = "";
        while(i < len) {
            size_t curr = get_utf8(code[i]);
            if(curr > 1) {
                for(size_t k = 0; k < curr; ++k) {
                    val += code[i];
                    i++;
                }
            }
            else if (i < len && (isdigit(code[i]) || isalpha(code[i]) 
            || code[i] == '_')) {
                val += code[i];
                i++;
            }
            else {
                break;
            }
        }
        T.KEY = TTYPE::STRING;
        T.VAL = val;
        tokens.push_back(T);
        continue;
	  }
	  if (current == '/' && i + 1 < len && code[i + 1] == '/') {
	    while(i < len && code[i] != '\n' ) { i++; }
	    continue;
	  }
	  if(current == '/' && i + 1 < len && code[i + 1] == '-') {
	    while(i + 1 < len && !(code [i] == '-' && code [i + 1] == '/')) { i++; }
	    if(i + 1 < len) {
	        i += 2;
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
            i++;
	    }
	    T.KEY = TTYPE::LOGIC_OPERATOR;
	    T.VAL = val;
	    tokens.push_back(T);
	    i++;
	    continue;
	  }
	  if(current == '!') {
	    string val(1,current);
	    T.KEY = TTYPE::NOT;
	    T.VAL = val;
	    tokens.push_back(T);
	    i++;
	    continue;
	  }
	  if((current == '&' && i + 1 < len && code[i+1] == '&') || (current == '|' && i + 1 < len && code[i+1] == '|')) {
	    string val = {code[i],code[i+1]};
	    T.KEY = TTYPE::BOOLEA_OPERATOR;
	    T.VAL = val;
	    tokens.push_back(T);
	    i += 2;
	    continue;
	  }
	  if (current == '+' || current == '-' || current == '*' ||
	      current == '/' || current == '=' || current == '%') {
	    string val(1, current);
	    T.KEY = TTYPE::OPERATOR;
	    T.VAL = val;
	    tokens.push_back(T);
	    i++;
	    continue;
	  }
	  if(current == ';' ) {
	  	string val(1,current);
	  	T.KEY = TTYPE::END_EX;
	  	T.VAL = val;
	  	tokens.push_back(T);
	  	i++;
	  	continue;
	  }
	  if (current == '[' || current == ']' ||
	      current == '(' || current == ')' || 
	      current == ',' || current == '{' ||
	      current == '}' || current == '.') {
	    string val(1, current);
	    T.KEY = TTYPE::SEPARATOR;
	    T.VAL = val;
	    tokens.push_back(T);
	    i++;
	    continue;
	  }
	  if(current == '"' || current == '\'' ) {
	    char quote = current;
	  	i++;
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
                        i += 2;
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
	  	                i += 2;
	  	            }else {
	  	                val += '\0';
	  	            }
	  	        }else {
	  	            val += '\\';
	  	            val += code[i + 1];
	  	        }
	  	        i += 2;
	  	        continue;
	  	    }
	  		val += code[i];
	  		i++;
	  	}
	  	if(i < len && code[i] == quote) {
		  	i++;
		  	T.KEY = TTYPE::STRING_LIT;
		  	T.VAL = val;
		  	tokens.push_back(T);
		}
		else {
			cout<<"\033[1;31mE: small dick on the quotes, expected (\033[0m"<<quote<<"\033[1;31m)\033[0m"<<endl;
			tokens.clear();
			return tokens;
		}
	  continue;
	  }
	  else {
	  	string val(1,current);
	  	T.KEY = TTYPE::UNKNOWN;
	  	T.VAL = val;
	  	tokens.push_back(T);
	  	i++; continue;
	  }
  }
  return tokens;
}
