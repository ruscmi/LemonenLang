/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include <iostream>
#include "../include/ast.hpp"
void print_tree(Node* node, unsigned int level) {
	if(node == nullptr) { return; }
	for(unsigned int i = 0; i < level; i++ ) { cout<<"   "; }
	cout<<node->VAL;
	if(node->KEY == ST_NUMBER ) { cout<<" [NUMBER]\n"; }
	else if(node->KEY == ST_OPERATOR) { cout<<" [OPERATOR]\n"; }
	else if(node->KEY == ST_VARIABLE) { cout<<" [VARIABLE]\n"; }
	else if(node->KEY == ST_ASSIGNMENT) { cout<<"[ASSIGNMENT]\n"; }
	else if(node->KEY == ST_PRINT) { cout<<"[PRINT]\n"; }
	else if(node->KEY == ST_STRING) { cout<<" [STRING]\n"; }
	else { cout<<" [UNKNOWN]"; }
	print_tree(node->left_index, level + 1);
	print_tree(node->right_index, level + 1);
}
