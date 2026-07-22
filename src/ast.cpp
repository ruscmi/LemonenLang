/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include <iostream>
#include "../include/ast.hpp"
#include "../include/utf8_win.hpp"
Node::~Node() {
    delete right_index;
    delete left_index;
    delete if_index;
    delete else_index;
    for(Node* node : children) {
        delete node;
    }
}
void print_tree(Node* node, unsigned int level) {
	setup_utf8();
	if(node == nullptr) { return; }
	for(unsigned int i = 0; i < level; i++ ) { cout<<"   "; }
	cout<<node->VAL;
	if(node->KEY == ST_NUMBER ) { cout<<" [NUMBER]\n"; }
	else if(node->KEY == ST_OPERATOR) { cout<<" [OPERATOR]\n"; }
	else if(node->KEY == ST_VARIABLE) { cout<<" [VARIABLE]\n"; }
	else if(node->KEY == ST_ASSIGNMENT) { cout<<"[ASSIGNMENT]\n"; }
	else if(node->KEY == ST_PRINT) { cout<<"[PRINT]\n"; }
	else if(node->KEY == ST_STRING) { cout<<" [STRING]\n"; }
	else if(node->KEY == ST_INDEX) { cout<<"[INDEX]\n"; }
	else if(node->KEY == ST_ARRAY) { cout<<"[ARRAY]\n"; }
	else { cout<<" [UNKNOWN]"; }
	print_tree(node->left_index, level + 1);
	print_tree(node->right_index, level + 1);
}
void print_array(const Value& val) {
    if (holds_alternative<double>(val)) {
        cout << get<double>(val);
    } else if (holds_alternative<string>(val)) {
        cout << get<string>(val);
    } else if (holds_alternative<shared_ptr<ArrayValue>>(val)) {
        auto arr = get<shared_ptr<ArrayValue>>(val);
        cout << "[";
        for (size_t i = 0; i < arr->elements.size(); ++i) {
            print_array(arr->elements[i]);
            if (i != arr->elements.size() - 1) cout << ", ";
        }
        cout << "]";
    } 
    else if(holds_alternative<ErrorValue>(val)) {
        cout<<"\033[1;31mE: unknown TTYPE to interpreter\033[0m"<<endl;
    } 
    else if(holds_alternative<AcceptValue>(val)) { }
    else {
        cout << "?";
    }
}
