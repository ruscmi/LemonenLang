#include "/home/smirnow/lmnlang_v2/include/lexer.hpp"
#include "/home/smirnow/lmnlang_v2/include/parser.hpp"
#include "/home/smirnow/lmnlang_v2/include/ast.hpp"
#include <iostream>
bool ActiveRequest = true;
int main() {
  LEX lexing;
  string code;
  while (ActiveRequest) {
    cout << "#> ";
    getline(cin, code);
    vector<Token>tokenize = lexing.tokenize(code);
    Parser p(tokenize);
    Node* tree = p.parse_program();
    cout<<"==AST-TREE=="<<endl;
    if(tree != nullptr) {
    	print_tree(tree, 0);
    } else {
    	cout<<"E: nullptr TRUE";
    }
    cout<<endl;
    cout<<"TOKENS: "<<tokenize.size()<<endl;
    if (code == "exit") {
      ActiveRequest = false;
      break;
    }
  }
}
