/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/lexer.hpp"
#include "../include/utf8_win.hpp"
#include <cctype>
#include <iostream>
std::vector<Token>& LEX::tokenize(const string &code) {
	setup_utf8();
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
	  if (isalpha(current)) {
	    string val = "";
	    while (i < len && (isdigit(code[i]) || isalpha(code[i]) 
	    || code[i] == '_')) {
	      val += code[i];
	      i++;
	    }
	    T.KEY = TTYPE::STRING;
	    T.VAL = val;
	    tokens.push_back(T);
	    continue;
	  }
	  if (current == '/' && 1 < len && code[i + 1] == '/') {
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
	  current == '!' || (current == '=' && i + 1 < len && code[i+1] == '=')) {
	    string val = "";
	    val += current;
	    if(i + 1 < len && code[i+1] == '=') {
            val += code[i+1];
            i++;
	    }
	    else if(val == "!") {
	        cout<<"\033[1;31mE: excepted one '!' in conditions\033[0m"<<endl;
	        T.KEY = TTYPE::UNKNOWN;
	        T.VAL = val;
	        tokens.push_back(T);
	        return tokens;
	    }
	    T.KEY = TTYPE::LOGIC_OPERATOR;
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
	      current == '/' || current == '=' ) {
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
	      current == '}') {
	    string val(1, current);
	    T.KEY = TTYPE::SEPARATOR;
	    T.VAL = val;
	    tokens.push_back(T);
	    i++;
	    continue;
	  }
	  if(current == '"' ) {
	  	i++;
	  	string val = "";
	  	while(i < len && code[i] != '"') {
	  	    if(code[i] == '\\' && i + 1 < len) {
	  	        if(code[i + 1] == 'n') {
	  	            val += '\n';
	  	            i += 2;
	  	            continue;
	  	        }
	  	    }
	  		val += code[i];
	  		i++;
	  	}
	  	if(i < len && code[i] == '"' ) {
		  	i++;
		  	T.KEY = TTYPE::STRING_LIT;
		  	T.VAL = val;
		  	tokens.push_back(T);
		}
		else {
			cout<<"\033[1mE: small dick on the quotes, expected '\"'\033[0m"<<endl;
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
