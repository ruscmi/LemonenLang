/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#pragma once
#include <string>
#include <vector>
using namespace std;
enum TTYPE { KEYWORD, OPERATOR, SEPARATOR, NUMBER, STRING, UNKNOWN,END};
struct Token {
  TTYPE KEY;
  string VAL;
};
enum ASTTAB { ST_ASSIGNMNET, ST_NUMBER , ST_OPERATOR, ST_VARIABLE, ST_SEPARATOR };
struct Node {
  ASTTAB KEY;
  string VAL;
  Node* left_index;
  Node* right_index;
};
void print_tree(Node* node,int level = 0);
