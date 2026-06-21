#include <cctype>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
bool ActiveRequest = true;
using namespace std;
#pragma region
enum TTYPE { KEYWORD, OPERATOR, SEPARATOR, NUMBER, STRING, UNKNOWN };
struct Token {
  TTYPE KEY;
  string VAL;
};
struct Node {
  TTYPE KEY;
  string VAL;
  int left_index;
  int right_index;
};
class LEX {
private:
  vector<Token> tokens;
  Token T;
public:
  vector<Token>& tokenize(const string &code) {
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
        string val(2, current);
        T.KEY = TTYPE::SEPARATOR;
        T.VAL = val;
        tokens.push_back(T);
        i++;
        continue;
      }
    }
    return tokens;
  }
  void vector_parsing() {
    for (const auto &token : tokens) {
      cout << token.KEY << " : " << token.VAL << " ";
    }
    cout << endl;
  }
};
class parser {
private:
    unsigned int position = 0;
    vector<Token> tokenize;
public:
	parser(vector<Token> tokenize){
		this->tokenize=tokenize;
	}
	Token peer() {
		if(position >= tokenize.size()) {
			return Token{TTYPE::UNKNOWN,""};
		}
		return tokenize[position];
	}
	Token advanced() {
		if(position < tokenize.size()) {
			position++; 
		}
		return tokenize[position -1];
	}
};
int main() {
  LEX lexing;
  string code;
  while (ActiveRequest) {
    cout << "#> ";
    getline(cin, code);
    lexing.tokenize(code);
    lexing.vector_parsing();
    if (code == "exit") {
      ActiveRequest = false;
      break;
    }
  }
}
#pragma endregion
