/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <variant>
using namespace std;
extern bool is_runner;
enum TTYPE { KEYWORD, OPERATOR, SEPARATOR, NUMBER, STRING, UNKNOWN,END,STRING_LIT,END_EX };
struct Token {
  TTYPE KEY;
  string VAL;
};
enum ASTTAB { ST_ASSIGNMENT, ST_NUMBER , ST_OPERATOR, ST_VARIABLE, ST_SEPARATOR, ST_PRINT, ST_STRING, 
ST_NOP, ST_INDEX, ST_ARRAY  };
struct Node {
  ASTTAB KEY;
  string VAL;
  Node* left_index;
  Node* right_index;
  vector<Node*> children;
  ~Node();
};
struct ArrayValue;
using Value = variant<double,string,shared_ptr<ArrayValue>>; 
struct ArrayValue {
    vector<Value>elements;
};
void print_tree(Node* node,unsigned int level = 0);
void print_array(const Value& val);
