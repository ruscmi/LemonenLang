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
    delete block_while;
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
	else if(node->KEY == ST_IF) { cout<<"[IF]\n"; }
	else if(node->KEY == ST_SEPARATOR) { cout<<"[SEPARATOR]\n"; }
	else if(node->KEY == ST_INPUT) { cout<<"[INPUT]\n"; }
	else if(node->KEY == ST_BLOCK) { cout<<"[BLOCK]\n"; }
	else if(node->KEY == ST_LOGIC_OPERATOR) { cout<<"[LOGIC_OPERATOR]\n"; }
	else if(node->KEY == ST_STOD) { cout<<"[STOD]\n"; }
	else if(node->KEY == ST_BOOLEA_OPERATOR) { cout<<"[BOOLEA_OPERATOR]\n";}
	else if(node->KEY == ST_BOOL) { cout<<"[BOOL]\n";}
	else if(node->KEY == ST_PROGRAM) { cout<<"[PROGRAM]\n"; }
	else if(node->KEY == ST_NOT) { cout<<"[NOT]\n"; }
	else if(node->KEY == ST_WHILE) { cout<<"[WHILE]\n"; }
	else if(node->KEY == ST_CONTINUE) { cout<<"[CONTINUE]\n"; }
	else if(node->KEY == ST_TYPEOF) { cout<<"[TYPEOF]\n"; }
	else if(node->KEY == ST_BREAK) { cout<<"[BREAK]\n"; }
	else if(node->KEY == ST_LEN) { cout<<"[LEN]\n"; }
	else if(node->KEY == ST_ARRAY_PUSH) { cout<<"[ARRAY_PUSH]\n"; }
	else if(node->KEY == ST_FUNC) { cout<<"[FUNCTION]\n"; }
	else if(node->KEY == ST_INCLUDE) { cout<<"[INCLUDE]\n"; }
	else if(node->KEY == ST_INCLUDE_LIBS) { cout<<"[INCLUDE_LIBS]\n"; }
	else if(node->KEY == ST_CALL) { cout<<"[CALL]\n"; }
	else if(node->KEY == ST_RETURN) { cout<<"[RETURN]\n"; }
	else if(node->KEY == ST_WAIT) { cout<<"[WAIT]\n"; }
	else if(node->KEY == ST_DICTIONARY) { cout<<"[DICTIONARY]\n"; }
	else { cout<<" [UNKNOWN]"; }
	for(Node* child : node->children) {
	    print_tree(child,level + 1);
	}
	print_tree(node->left_index, level + 1);
	print_tree(node->right_index, level + 1);
}
void print_array(const Value& val) {
    if (holds_alternative<double>(val)) {
        cout << get<double>(val);
    }else if (holds_alternative<string>(val)) {
        cout << get<string>(val);
    }else if(holds_alternative<bool>(val)) {
        cout << get<bool>(val);
    }else if (holds_alternative<shared_ptr<ArrayValue>>(val)) {
        auto arr = get<shared_ptr<ArrayValue>>(val);
        cout << "[";
        for (size_t i = 0; i < arr->elements.size(); ++i) {
            print_array(arr->elements[i]);
            if (i != arr->elements.size() - 1) cout << ", ";
        }
        cout << "]";
    }
    else if(holds_alternative<shared_ptr<DictValue>>(val)) {
        auto dict = get<shared_ptr<DictValue>>(val);
        cout << "{";
        bool first = true;
        for(const auto& [key,val] : dict->dict_val) {
            if(!first) {
                cout<<", ";
            }
            first = false;
            cout<<"'"<<key<<"' : ";
            print_array(val);
        }
        cout<<"}";
    } 
    else if(holds_alternative<ErrorValue>(val)) {
        cout<<"\033[1;31mE: unknown TTYPE to interpreter\033[0m";
    }
    else if(holds_alternative<AcceptValue>(val)) { }
    else if(holds_alternative<Breaker>(val)) { }
    else if(holds_alternative<Continuer>(val)) { }
    else {
        cout << "?";
    }
}
