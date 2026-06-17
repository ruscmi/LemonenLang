#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>
using namespace std;
#pragma region
enum TTYPES { KEYWORD, OPERATOR, SEPARATOR, NUMBER, STRING, UNKNOWN };
struct Token { TTYPES KEY; string VAL; };
class LEX{
public:
    vector<Token>tokenize(const string& code) {
    	vector<Token> tokens;
    	unsigned int i=0;
    	unsigned int len = code.length();
        string val = "";
    	while( i < len ) { 
    	    unsigned char current = code[i];
    	    if(isspace(current)) { i++; continue; } 
    	    if(isdigit(current)) { Token T; while(i < len && isdigit(code[i])){ current = current + code[i]; i++;  } tokens.push_back({TTYPES::NUMBER,val }); continue; }
    	}
    	return tokens;
    }    
};
int main() { 
        string code; LEX Lexer cout<<"󰣇 󰁔"; getline(cin,code) 
        
    }
#pragma endregion
