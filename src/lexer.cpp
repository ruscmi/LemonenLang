/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/lexer.hpp"
#include <cctype>
#include <iostream>
std::vector<Token>& LEX::tokenize(const string &code) {
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
	    string val(1, current);
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
	  	i++; continue;
	  }
  }
  return tokens;
}
