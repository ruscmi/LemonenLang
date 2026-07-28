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
enum TTYPE { OPERATOR, SEPARATOR, NUMBER, STRING, UNKNOWN,END,STRING_LIT,END_EX,LOGIC_OPERATOR,BOOLEA_OPERATOR,NOT};
struct Token {
  TTYPE KEY;
  string VAL;
};
enum ASTTAB { ST_ASSIGNMENT, ST_NUMBER , ST_OPERATOR, ST_VARIABLE, ST_SEPARATOR, ST_PRINT, ST_STRING, 
ST_NOP, ST_INDEX, ST_ARRAY,ST_INPUT,ST_BLOCK,ST_IF,ST_LOGIC_OPERATOR,ST_STOD,ST_BOOLEA_OPERATOR,ST_BOOL,
ST_PROGRAM,ST_NOT,ST_WHILE,ST_CONTINUE,ST_BREAK,ST_TYPEOF,ST_LEN,ST_ARRAY_PUSH,ST_INCLUDE,ST_INCLUDE_LIBS};
struct Node {
  ASTTAB KEY;
  string VAL;
  Node* left_index;
  Node* right_index;
  vector<Node*> children;
  Node* if_index;
  Node* else_index;
  Node* block_while;
  ~Node();
};
struct ErrorValue {
    string message;
    bool operator==(const ErrorValue& other) const = default;
};
struct Continuer { bool operator==(const Continuer& other) const = default; };
struct Breaker { bool operator==(const Breaker& other) const = default; };
struct AcceptValue { bool operator==(const AcceptValue& other) const = default; };
struct ArrayValue;
using Value = variant<double,bool,string,shared_ptr<ArrayValue>,ErrorValue,AcceptValue,Continuer,Breaker>; 
struct ArrayValue {
    vector<Value>elements;
    void push(const Value& val) {
        elements.push_back(val);
    }
    bool operator==(const ArrayValue& other) const {
        return elements == other.elements;
    }
    bool operator!=(const ArrayValue& other) const {
        return !(*this == other);
    }
};
void print_tree(Node* node,unsigned int level = 0);
void print_array(const Value& val);
