/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include "../include/utf8_win.hpp"
#include "../include/ast.hpp"
#include "../src/manual.cpp"
#include "../include/interpreter.hpp"
#include <readline/readline.h>
#include <format>
#include <iostream>
bool is_runner = false;
void Parser::setTokens(const vector<Token>& tokenize) {
	this->tokenize = tokenize;
	this->position = 0;
}
Token Parser::peer() {
	if(position >= tokenize.size()) {
		return Token{TTYPE::END,""};
	}
	return tokenize[position];
}
Token Parser::advanced() {
	if(position < tokenize.size()) {
		position++; 
	}
	return tokenize[position -1];
}
Node* Parser::parse_program() {
	setup_utf8();
	for(const auto& token : tokenize ) {
		if(token.KEY == TTYPE::UNKNOWN ) {
			cout<<"\033[1;31mE: Deer,my parser doesn't understand this shit\033[0m"<<endl;
			return nullptr;
		}
	}
	Node* end = new Node();
	end->KEY = ST_PROGRAM;
	while(peer().KEY != TTYPE::END) {
        Node* expr = parse_statement();
        if(!expr) { 
            delete end; return nullptr;
        }
        end->children.push_back(expr);
        
        if(peer().KEY == TTYPE::END_EX) {
            advanced();
        }
        else if(peer().KEY != TTYPE::END) {
            cout<<"\033[1;33mE: expected ';' or end of input\033[0m"<<endl;
            delete expr;
            return nullptr;
        }
    }
    return end;
}
Node* Parser::parse_len() {
    setup_utf8();
    Node* lmlen = new Node();
    lmlen->KEY = ST_LEN;
    lmlen->VAL = "lmlen";
    lmlen->left_index = nullptr;
    lmlen->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            Node* expr = parse_expression();
            lmlen->right_index = expr;
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                cout<<"\033[1;33mE: expected ')' in lmlen \033[0m"<<endl;
                delete lmlen;
                return nullptr;
            }    
        }
        return lmlen; 
    }else {
        cout<<"\033[1;33mE: expected '(' in lmlen \033[0m"<<endl;
        delete lmlen;
        return nullptr;
    }
    return nullptr;
}
Node* Parser::parse_typeof() {
    setup_utf8();
    Node* lmtype = new Node();
    lmtype->KEY = ST_TYPEOF;
    lmtype->VAL = "lmtype";
    lmtype->left_index = nullptr;
    lmtype->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            Node* expr = parse_expression();
            lmtype->right_index = expr;
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                cout<<"\033[1;33mE: expected ')' in lmtype \033[0m"<<endl;
                delete lmtype;
                return nullptr;
            }    
        }
        return lmtype; 
    }else {
        cout<<"\033[1;33mE: expected '(' in lmtype \033[0m"<<endl;
        return nullptr;
    }
    return nullptr;    
}
Node* Parser::parse_include() {
    setup_utf8();
    advanced();
    if(peer().KEY == TTYPE::STRING_LIT) {
        string val = peer().VAL;
        if(val.empty()) {
            cout<<"\033[1;31mE: this file returning null you stupid man?\033[0m"<<endl;
            return nullptr;
        }
        advanced();
        Node* files = new Node();
        files->KEY = ST_INCLUDE;
        files->VAL = val;
        return files;
    }
    else if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY != TTYPE::STRING) {
            cout<<"\033[1;31mE: returning TTYPE in parentheses is not a string\033[0m"<<endl;
            return nullptr;
        }
        string val = peer().VAL;
        if(val.empty()) {
            cout<<"\033[1;31mE: this library there is no bitch\033[0m"<<endl;
            return nullptr;
        }
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();
            Node* lib = new Node();
            lib->KEY = ST_INCLUDE_LIBS;
            lib->VAL = val;
            return lib;
        }else {
            cout<<"\033[1;31mE: not closed fucking parentheses\033[0m"<<endl;
            return nullptr;
        }
    }else {
        cout<<"\033[1;31mE: only support quotes(for stupid mans quotes - '\"') and parentheses\033[0m"<<endl;
        return nullptr;
    }
    return nullptr;
}
Node* Parser::parse_break() {
    if(peer().KEY == TTYPE::STRING && peer().VAL == "break") {
        setup_utf8();
        advanced();
        Node* node = new Node();
        node->KEY = ST_BREAK;
        node->VAL = "break";
        return node;
    }
    return nullptr;
}
Node* Parser::parse_continue() {
    setup_utf8();
    if(peer().KEY == TTYPE::STRING && peer().VAL == "continue") {
        advanced();
        Node* node = new Node();
        node->KEY = ST_CONTINUE;
        node->VAL = "continue";
        return node;
    }
    return nullptr;
}
Node* Parser::parse_while() {
    setup_utf8();
    advanced();
    Token current = peer();
    Node* then_node = nullptr;
    Node* cond = nullptr;
    if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(") {
        advanced();
        cond = parse_boolea_expression();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
            advanced();
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
                advanced();
                then_node = new Node();
                then_node->KEY = ST_BLOCK;
                while(peer().KEY != TTYPE::SEPARATOR && peer().VAL != "}") {
                    Node* expr = parse_statement();
                    if(expr) {
                        then_node->children.push_back(expr);
                    }else {
                        cout<<"\033[1;31mE: expected statement logic in ST_BLOCK\033[0m"<<endl;
                        delete then_node;
                        delete cond;
                        return nullptr;
                    }
                    if(peer().KEY == TTYPE::END_EX) {
                        advanced();
                    }
                    if(peer().KEY == TTYPE::END) {
                        cout<<"\033[1;31mE: expected '}' in ST_BLOCK\033[0m"<<endl;
                        delete then_node;
                        delete cond;
                        return nullptr;
                    }
                }
                advanced();
            }else {
                    then_node = parse_statement();
                    cout<<"\033[1;31mE: expected TTYPE::UNKNOWN in term\033[0m"<<endl;
                    delete cond;
                    return nullptr;
            }
        }else {
            cout<<"\033[1;31mE: expected sucked ')' or detected extra neurons\033[0m"<<endl;
            delete cond;
            return nullptr;
        }
    }else {
        cout<<"\033[1;31mE: expected sucked '(' or detected extra chromosomes\033[0m"<<endl;
        return nullptr;
    }
    Node* while_node = new Node();
    while_node->KEY = ST_WHILE;
    while_node->VAL = "while";
    while_node->left_index = cond;
    while_node->block_while = then_node;
    return while_node;
}
Node* Parser::parse_bool() {
    setup_utf8();
    Token current = peer();
    if(current.KEY == TTYPE::STRING && current.VAL == "true") {
        Node* true_f = new Node();
        true_f->KEY = ST_BOOL;
        true_f->VAL = "true";
        advanced();
        return true_f;
    }
    if(current.KEY == TTYPE::STRING && current.VAL == "false") {
        Node* false_f = new Node();
        false_f->KEY = ST_BOOL;
        false_f->VAL = "false";
        advanced();
        return false_f;
    }
    return nullptr;
}
Node* Parser::parse_if() {
    setup_utf8();
    advanced();
    Token current = peer();
    Node* then_node = nullptr;
    Node* cond = nullptr;
    if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(") {
        advanced();
        cond = parse_boolea_expression();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
            advanced();
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
                advanced();
                then_node = new Node();
                then_node->KEY = ST_BLOCK;
                while(peer().KEY != TTYPE::SEPARATOR && peer().VAL != "}") {
                    Node* expr = parse_statement();
                    if(expr) {
                        then_node->children.push_back(expr);
                    }else {
                        cout<<"\033[1;31mE: expected statement logic in ST_BLOCK\033[0m"<<endl;
                        delete then_node;
                        delete cond;
                        return nullptr;
                    }
                    if(peer().KEY == TTYPE::END_EX) {
                        advanced();
                    }
                    if(peer().KEY == TTYPE::END) {
                        cout<<"\033[1;31mE: expected '}' in ST_BLOCK\033[0m"<<endl;
                        delete then_node;
                        delete cond;
                        return nullptr;
                    }
                }
                advanced();
            }else {
                then_node = parse_statement();
                if(!then_node) {
                    cout<<"\033[1;31mE: expected TTYPE::UNKNOWN in term\033[0m"<<endl;
                    delete cond;
                    return nullptr;
                }
            }
        }else {
            cout<<"\033[1;31mE: expected sucked ')' or detected extra neurons\033[0m"<<endl;
            delete cond;
            return nullptr;
        }
    }else {
        cout<<"\033[1;31mE: expected sucked '(' or detected extra chromosomes\033[0m"<<endl;
        return nullptr;
    }
    Node* else_node = nullptr;
    if(peer().KEY == TTYPE::STRING && peer().VAL == "else") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
            advanced();
            else_node = new Node();
            else_node->KEY = ST_BLOCK;
            while(peer().KEY != TTYPE::SEPARATOR && peer().VAL != "}") {
                Node* expr = parse_statement();
                if(expr) {
                    else_node->children.push_back(expr);
                }else {
                    cout<<"\033[1;31mE: expected statement logic in ST_BLOCK\033[0m"<<endl;
                    delete else_node;
                    delete then_node;
                    delete cond;
                    return nullptr;
                }
                if(peer().KEY == TTYPE::END_EX) {
                    advanced();
                }
                if(peer().KEY == TTYPE::END) {
                    cout<<"\033[1;31mE: expected '}' in ST_BLOCK\033[0m"<<endl;
                    delete else_node;
                    delete then_node;
                    delete cond;
                    return nullptr;
                }
            }
            advanced();
        }else {
            else_node = parse_statement();
            if(!else_node) {
                cout<<"\033[1;31mE: expected TTYPE::UNKNOWN in term\033[0m"<<endl;
                delete then_node;
                delete cond;
                return nullptr;
            }
        }
    }
    Node* if_node = new Node();
    if_node->KEY = ST_IF;
    if_node->VAL = "if";
    if_node->left_index = cond;
    if_node->if_index = then_node;
    if_node->else_index = else_node;
    return if_node;
}
Node* Parser::parse_print() {
	setup_utf8();
	advanced();
	Node* print_node = new Node();
	print_node->KEY = ST_PRINT;
	print_node->VAL = "lmuck";
	print_node->left_index = nullptr;
	print_node->right_index = nullptr;
	if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
	    advanced();
	    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
	        advanced();
	        return print_node;
	    }
    	Node* expr = parse_expression();
    	if(expr == nullptr) {
    		cout<<"\033[1;33mE: you didn't add the arguments fucked mudda\033[0m"<<endl;
            delete print_node;
    		return nullptr;
    	}
    	print_node->children.push_back(expr);
    	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
            advanced();
            Node* t_expr = parse_expression();
            if(!t_expr) {
                cout<<"\033[1;33mE: you didn't add the arguments fucked mudda\033[0m"<<endl;
                return nullptr;
            }
            print_node->children.push_back(t_expr);
        }
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();
            return print_node;
        }else {
            cout<<"\033[1;33mE: expected ')' in lmuck()\033[0m"<<endl;
            delete print_node;
            return nullptr;
        }
    }else {
        cout<<"\033[1;33mE: expected '(' after lmuck()\033[0m"<<endl;
        delete print_node;
        return nullptr;
    }
}
Node* Parser::parse_stod() {
    Node* lmtod = new Node();
    lmtod->KEY = ST_STOD;
    lmtod->VAL = "lmtod";
    lmtod->left_index = nullptr;
    lmtod->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            Node* expr = parse_expression();
            lmtod->right_index = expr;
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                cout<<"\033[1;33mE: expected ')' in lmtod \033[0m"<<endl;
                delete lmtod;
                return nullptr;
            }    
        }
        return lmtod; 
    }else {
        cout<<"\033[1;33mE: expected '(' in lmtod \033[0m"<<endl;
        return nullptr;
    }
    return nullptr;
}
Node* Parser::parse_input() {
    Node* input_node = new Node();
    input_node->KEY = ST_INPUT;
    input_node->VAL = "lmout";
    input_node->left_index = nullptr;
    input_node->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            Node* expr = parse_expression();
            input_node->right_index = expr;
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                cout<<"\033[1;33mE: expected ')' in lmout \033[0m"<<endl;
                delete input_node;
                return nullptr;
            }    
        }
        return input_node; 
    }else {
        cout<<"\033[1;33mE: expected '(' in lmout \033[0m"<<endl;
        return nullptr;
    }
    return nullptr;
}
Node* Parser::parse_statement() {
	Token current = peer();
	if (current.KEY == TTYPE::STRING && current.VAL == "lmuck") {
	    return parse_print();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "man") {
		return parse_manual();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "if") {
	    return parse_if();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "while") {
	    return parse_while();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "break") {
	    return parse_break();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "continue") {
	    return parse_continue();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "lmport") {
	    return parse_include();
	}
	else if(current.KEY == TTYPE::STRING) {
	    bool is_assignment = false;
	    size_t pos_expr = position;
	    while(pos_expr < tokenize.size() && tokenize[pos_expr].KEY != TTYPE::END_EX) {
	        if(tokenize[pos_expr].KEY == TTYPE::OPERATOR && tokenize[pos_expr].VAL == "=") {
	            is_assignment = true;
	            break;
	        }
	        pos_expr++;
	    }
	    if(is_assignment) {
	        return parse_assignment();
	    }else {
	        return parse_expression();
	    }
	}
	else {
		return parse_expression();
	}
}
Node* Parser::parse_assignment() {
    vector<Node*>elements;
    while(true) {
        if(peer().KEY != TTYPE::STRING) {
            cout<<"\033[1;33mE: peer().KEY != TTYPE::STRING(thong)\033[0m"<<endl;
            return nullptr;
        }
        Node* var_node = new Node();
        var_node->KEY = ST_VARIABLE;
        var_node->VAL = peer().VAL;
        advanced();
        elements.push_back(var_node);
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
            advanced();
            continue;
        }
        else if(peer().KEY == TTYPE::OPERATOR && peer().VAL == "=") {
            break;
        }
        else {
            cout<<"\033[1;33mE: UNKNOWN::TTYPE in assignmentation  \033[0m"<<endl;
            return nullptr; 
        }
    }
    advanced();
    Node* right_expr = parse_expression();
    if(!right_expr) {
        cout<<"\033[1;33mE: expected TTYPE::OPERATOR \"=\" \033[0m"<<endl;
        for(Node* node : elements) {
            delete node;
        }
        return nullptr;
    }
    Node* assign = new Node();
    assign->KEY = ST_ASSIGNMENT;
    assign->VAL = "=";
    assign->left_index = nullptr;
    assign->right_index = right_expr;
    assign->children = elements;
    return assign;
}
Node* Parser::parse_factor() {
	setup_utf8();
	Token current = peer();
	Node* left = nullptr;
    if(current.KEY == TTYPE::NUMBER) {
		Node* node = new Node();
		node->KEY = ST_NUMBER;
		node->VAL = current.VAL;
		advanced();
		left = node;
	}
	else if(current.KEY == TTYPE::STRING) {
	    if(current.VAL == "lmout") {
	        advanced();
            left = parse_input();
	    }
	    else if(current.VAL == "lmtod") {
	        advanced();
	        left = parse_stod();
	    }
        else if(current.VAL == "lmtype") {
            advanced();
            left = parse_typeof();
        }
        else if(current.VAL == "lmlen") {
            advanced();
            left = parse_len();
        }
        else if(current.VAL == "lmuck") {
            left = parse_print();
        }
        else if(current.VAL == "true" || current.VAL == "false" ) {
            left = parse_bool();
        }
	    else {
    		Node* node = new Node();
    		node->KEY = ST_VARIABLE;
    		node->VAL = current.VAL;
    		advanced();
    		left = node;
    		if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
                advanced();
                Node* expr = parse_expression();
                if(!expr) {
                    cout<<"\033[1;31mE: expected expression in variable index\033[0m"<<endl;
                    delete node;
                    return nullptr;
                }
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                    advanced();
                }else {
                    cout<<"\033[1;31mE: expected ')' in variable index\033[0m"<<endl;
                    delete node;
                    delete expr;
                    return nullptr;
                }
    		    Node* index = new Node();
    		    index->KEY = ST_INDEX;
    		    index->VAL = peer().VAL;
    		    index->left_index = node;
    		    index->right_index = expr;
    		    left = index;
    		}else {
    		    left = node;
    		}
        }
	}
    else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "[") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
            advanced();
            Node* node = new Node();
            node->KEY = ST_ARRAY;
            node->children = {};
            left = node;
            return left;
        }
        if(peer().KEY == TTYPE::END) {
            return nullptr;
        }
        vector<Node*>elements;
        while(true) {
            Node* element = parse_expression();
            if(!element) {
                cout<<"\033[1;33mE: cant parsing array,detection TTYPE::UNKNOWN in array\033[0m"<<endl;
                return nullptr;
            }
            elements.push_back(element);
            
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
                advanced();
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                    cout<<"\033[1;33mE: you shit it, the next comma in the array is forbidden\033[0m"<<endl;
                    return nullptr;
                }
                continue;
            }
            else if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                break;
            }else {
                cout<<"\033[1;33mE: expected stupid parenthesis in fucked ST_ARRAY\033[0m"<<endl;
                return nullptr;
            }
        }
        Node* node = new Node();
        node->KEY = ST_ARRAY;
        node->children = elements;
        advanced();
        left = node;
    }
	else if(current.KEY == TTYPE::STRING_LIT) {
		Node* node = new Node();
		node->KEY = ST_STRING;
		node->VAL = current.VAL;
		advanced();
		left = node;
	}
	else if(current.KEY == TTYPE::END_EX) {
		advanced();
    	return nullptr;
	}
	else if(current.KEY == TTYPE::NOT) {
	    advanced();
	    Node* right = parse_factor();
	    Node* not_v = new Node();
	    not_v->KEY = ST_NOT;
	    not_v->VAL = "!";
	    not_v->left_index = nullptr;
	    not_v->right_index = right;
	    left = not_v;
	}
	else if(current.KEY == TTYPE::OPERATOR && current.VAL == "-") {
		advanced();
		Node* right = parse_factor();
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->VAL = "u-";
		node->left_index = nullptr;
		node->right_index = right;
		left = node;
	}
	else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(" ) {
		advanced();
		Node* inner = parse_expression();
		if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
			advanced();
			return inner;
		}
		else {
			cout<<"\033[1;33mE: small tits on the brackets, expected ')'\033[0m"<<endl;
			return nullptr;
		}
	}
	else {
		return nullptr;
	}
	if(!left) {return nullptr;}
	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "[") {
	    advanced();
	    Node* expr = parse_expression();
	    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
	        advanced();
     	    Node* node = new Node();
     	    node->KEY = ST_INDEX;
     	    node->left_index = left;
     	    node->right_index = expr;
     	    left = node;
	    }else {
	        cout<<"\033[1;33mE: expected your brain or ']' in index\033[0m"<<endl;
	        return nullptr;
	    }
	}
	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ".") {
	    advanced();
	    if(peer().KEY == TTYPE::STRING && peer().VAL == "lmpush") {
	        advanced();
	        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
	            advanced();
	            Node* expr = parse_expression();
	            if(!expr) {
	                cout<<"\033[1;31mE: expected expression in array lmpush\033[0m"<<endl;
	                return nullptr;
	            }
	            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
	                advanced();
	                Node* pusher = new Node();
	                pusher->KEY = ST_ARRAY_PUSH;
	                pusher->VAL = peer().VAL;
	                pusher->left_index = left;
	                pusher->right_index = expr;
	                left = pusher;
	            }else {
	                cout<<"\033[1;31mE: expected ')' in array lmpush\033[0m"<<endl;
	                delete expr;
	                return nullptr;
	            }
	        }
	    }
	}
	return left;
}
Node* Parser::parse_boolea_expression() {
	Node* left = parse_logic_expression();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::BOOLEA_OPERATOR &&  (current.VAL == "&&" || current.VAL == "||")) {
		const string current_op = current.VAL;
		advanced();
		Node* right = parse_logic_expression();
		if(right == nullptr) {
		    cout <<"\033[1;33mE: missing right operand for boolea operator '" << current_op << "'\033[0m" << endl;
		    if(left != nullptr) {
		        delete left;
		    }
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_BOOLEA_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;
		current = peer();
	}
	return left;
}
Node* Parser::parse_logic_expression() {
    Node* left = parse_expression();
    if(left == nullptr) {
        return nullptr;
    }
    while(peer().KEY == TTYPE::LOGIC_OPERATOR && (peer().VAL == ">" || peer().VAL == "<" 
    || peer().VAL == "!=" || peer().VAL == "==" || peer().VAL == "<=" || peer().VAL == ">=")) {
        const string current_op = peer().VAL;
		advanced();
		Node* right = parse_term();
		if(right == nullptr) {
		    cout <<"\033[1;33mE: missing right operand for logic operator '" << current_op << "'\033[0m" << endl;
		    if(left != nullptr) {
		        delete left;
		    }
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_LOGIC_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;    
    }
    return left;
}
Node* Parser::parse_term() {
	Node* left = parse_factor();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR &&  (current.VAL == "*" || current.VAL == "/" || current.VAL == "%")) {
		const string current_op = current.VAL;
		advanced();
		Node* right = parse_factor();
		if(right == nullptr) {
		    cout <<"\033[1;33mE: missing right operand for operator '" << current_op << "'\033[0m" << endl;
		    if(left != nullptr) {
		        delete left;
		    }
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;
		current = peer();
	}
	return left;
}
Node* Parser::parse_expression() {
	Node* left = parse_term();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR && (current.VAL == "+" || current.VAL == "-")) {
		const string current_op = current.VAL;
		advanced();
		Node* right = parse_term();
		if(right == nullptr) {
		    cout <<"\033[1;33mE: missing right operand for operator '" << current_op << "'\033[0m" << endl;
		    if(left != nullptr) {
		        delete left;
		    }
			return nullptr;
		}
		Node* node = new Node();
		node->KEY = ST_OPERATOR;
		node->left_index = left;
		node->right_index = right;
		node->VAL = current_op;
		left = node;
		current = peer();
	}
	return left;
}
