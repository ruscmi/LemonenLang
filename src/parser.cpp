/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include "../include/ast.hpp"
#include "../include/interpreter.hpp"
#include <format>
#include <iostream>
using namespace std;
bool is_runner = false;
void Parser::setTokens(const vector<Token>& tokenize) {
	this->tokenize = tokenize;
	this->position = 0;
}
Token Parser::peer() {
	if(position >= tokenize.size()) {
	    size_t line = tokenize.empty() ? 1 : tokenize.back().LINE;
	    size_t col = tokenize.empty() ? 1 : tokenize.back().COL;
		return Token{TTYPE::END,"",line,col};
	}
	return tokenize[position];
}
Token Parser::advanced() {
	if(position < tokenize.size()) {
		position++; 
	}
	return tokenize[position -1];
}
void Parser::error(const string& msg) {
    Token current = peer();
    cerr<<"\033[1;31m[e]: "<<msg<<"\033[0m"<<endl;
    if(!current.VAL.empty()) {
        cerr<<" "<<current.LINE<<" |\n   | '"<<current.VAL<<"'"<<endl;
        cerr<<"\033[1;34m     ^----------"<<"this in col:\033[0m"<<"["<<current.COL<<"]"<<endl;
    }else {
        cerr<<" "<<current.LINE<<" |\n   |"<<" "<<endl;
        cerr<<"\033[1;34m     ^---unexpected input in col:\033[0m"<<"["<<current.COL<<"]"<<endl;
    }
}
unique_ptr<Node> Parser::parse_program() {
	for(const auto& token : tokenize ) {
		if(token.KEY == TTYPE::UNKNOWN ) {
			error("Deer,my parser doesn't understand this shit");
			return nullptr;
		}
	}
	auto end = make_unique<Node>(peer());
	end->KEY = ST_PROGRAM;
	while(peer().KEY != TTYPE::END) {
        unique_ptr<Node> expr = parse_statement();
        if(expr == nullptr) { 
            return nullptr;
        }
        end->children.push_back(move(expr));
        
        if(peer().KEY == TTYPE::END_EX) {
            advanced();
        }
        else if(peer().KEY != TTYPE::END) {
            error("expected ';' or end of input");
            return nullptr;
        }
    }
    return end;
}
unique_ptr<Node> Parser::parse_for() {
    advanced();
    auto for_node = make_unique<Node>(peer());
    for_node->KEY = ST_FOR;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
    }else {
        error("expected '(' in for cycle");
        return nullptr;
    }
    unique_ptr<Node> one_ex = parse_assignment();
    if(!one_ex) {
        error("expected one argument in for cycle");
        return nullptr;
    }
    if(peer().KEY == TTYPE::END_EX) {
        advanced();
    }else {
        error("expected ';' in for arguments");
        return nullptr;
    }
    unique_ptr<Node> two_ex = parse_boolea_expression();
    if(!two_ex) {
        error("expected two argument in for cycle");
        return nullptr;
    }
    if(peer().KEY == TTYPE::END_EX) {
        advanced();
    }else {
        error("expected ';' in for arguments");
        return nullptr;
    }
    unique_ptr<Node> three_ex = parse_assignment();
    if(!three_ex) {
        error("expected three argument in for cycle");
    }
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
        advanced();
    }else {
        error("expected ')' in for cycle");
        return nullptr;
    }
    unique_ptr<Node> then_node;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
        advanced();
        then_node = make_unique<Node>(peer());
        then_node->KEY = ST_BLOCK;
        while(peer().KEY != TTYPE::SEPARATOR && peer().VAL != "}") {
            unique_ptr<Node>expr = parse_statement();
            if(expr) {
                then_node->children.push_back(move(expr));
            }else {
                error("expected statement logic in ST_BLOCK");
                return nullptr;
            }
            if(peer().KEY == TTYPE::END_EX) {
                advanced();
            }
            if(peer().KEY == TTYPE::END) {
                error("expected TTYPE::END in ST_BLOCK");
                return nullptr;
            }
        }
        advanced();
    }else {
        then_node = parse_statement();
        if(!then_node) {
            error("expected UNKNOWN in for cycle block");
            return nullptr;
        }
    }
    for_node->children.push_back(move(one_ex));
    for_node->children.push_back(move(two_ex));
    for_node->children.push_back(move(three_ex));
    for_node->children.push_back(move(then_node));
    return for_node;
}
unique_ptr<Node> Parser::parse_wait() {
    auto lmit = make_unique<Node>(peer());
    lmit->KEY = ST_WAIT;
    lmit->VAL = "lmit";
    lmit->left_index = nullptr;
    lmit->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            unique_ptr<Node>expr = parse_expression();
            lmit->right_index = move(expr);
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                error("expected ')' in lmit");
                return nullptr;
            }    
        }
        return lmit; 
    }else {
        error("expected '(' in lmit");
        return nullptr;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_func() {
    unique_ptr<Node> expr = nullptr;
    unique_ptr<Node>then_node = nullptr;
    vector<unique_ptr<Node>>args;
    if(peer().KEY == TTYPE::STRING) {
        string val = peer().VAL;
        advanced();
        if(peer().VAL.empty()) {
            error("expression is empty");
            return nullptr;
        }
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
            advanced();
            while(peer().VAL != ")") {
                if(peer().KEY != TTYPE::STRING) {
                    error("unknown pussy in the conditions");
                    return nullptr;
                }
                auto arg = make_unique<Node>(peer());
                arg->KEY = ST_VARIABLE;
                arg->VAL = peer().VAL;
                advanced();
                if(peer().KEY == TTYPE::OPERATOR && peer().VAL == "=") {
                    advanced();
                    expr = parse_logic_expression();
                    if(expr) {
                        arg->right_index = move(expr);
                    }else {
                        error("!expr error arg->right_index = expr");
                        return nullptr;
                    }
                }
                args.push_back(move(arg));
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
                    advanced();
                }else if(peer().VAL != ")") {
                    error("unclosed Fuck Bracket");
                    return nullptr;    
                }
            }
            advanced();
        }else {
            error("expected parentheses '('");
            return nullptr;
        }
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
            advanced();
            then_node = make_unique<Node>(peer());
            then_node->KEY = ST_BLOCK;
            while(peer().VAL != "}") {
                if(peer().KEY == TTYPE::END) {
                    error("TTYPE::END in block return null");
                    return nullptr;
                }
                if(peer().KEY == TTYPE::END_EX) {
                    advanced();
                }
                expr = parse_statement();
                if(expr) {
                    then_node->children.push_back(move(expr));
                }else {
                    error("!expr error then_node->children.push_back(expr)");
                    return nullptr;
                }
            }
            advanced();
            auto func = make_unique<Node>(peer());
            func->KEY = ST_FUNC;
            func->VAL = val;
            func->children = move(args);
            func->right_index = move(then_node);
            return func;
        }else {
            return nullptr;
        }
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_return() {
    advanced();
    auto returner = make_unique<Node>(peer());
    returner->KEY = ST_RETURN;
    returner->right_index = parse_boolea_expression();
    if(!returner->right_index) {
        error("returning null zero value in returner->right_index");
        return nullptr;
    }
    if(peer().KEY == TTYPE::END_EX) {
        advanced();
    }
    return returner;
}
unique_ptr<Node> Parser::parse_len() {
    auto lmlen = make_unique<Node>(peer());
    lmlen->KEY = ST_LEN;
    lmlen->VAL = "lmlen";
    lmlen->left_index = nullptr;
    lmlen->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            unique_ptr<Node>expr = parse_expression();
            lmlen->right_index = move(expr);
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                error("expected ')' in lmlen ");
                return nullptr;
            }    
        }
        return lmlen; 
    }else {
        error("expected '(' in lmlen ");
        return nullptr;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_typeof() {
    auto lmtype = make_unique<Node>(peer());
    lmtype->KEY = ST_TYPEOF;
    lmtype->VAL = "lmtype";
    lmtype->left_index = nullptr;
    lmtype->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            unique_ptr<Node> expr = parse_expression();
            lmtype->right_index = move(expr);
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                error("expected ')' in lmtype ");
                return nullptr;
            }    
        }
        return lmtype; 
    }else {
        error("expected '(' in lmtype ");
        return nullptr;
    }
    return nullptr;    
}
unique_ptr<Node> Parser::parse_include() {
    advanced();
    if(peer().KEY == TTYPE::STRING_LIT) {
        string val = peer().VAL;
        if(val.empty()) {
            error("this file returning null you stupid man?");
            return nullptr;
        }
        if(peer().KEY == TTYPE::END) {
            error("returning TTYPE::END fucked shiting man");
            return nullptr;
        }
        advanced();
        auto files = make_unique<Node>(peer());
        files->KEY = ST_INCLUDE;
        files->VAL = val;
        files->right_index = nullptr;
        files->left_index = nullptr;
        return files;
    }
    else if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY != TTYPE::STRING) {
            error("returning TTYPE in parentheses is not a string");
            return nullptr;
        }
        if(peer().KEY == TTYPE::END) {
            error("returning TTYPE::END fucked shiting man");
            return nullptr;
        }
        if(peer().KEY == TTYPE::SEPARATOR) {
            return nullptr;
        }
        string val = peer().VAL;
        if(val.empty()) {
            error("this library there is no bitch");
            return nullptr;
        }
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();
            auto lib = make_unique<Node>(peer());
            lib->KEY = ST_INCLUDE_LIBS;
            lib->VAL = val;
            lib->right_index = nullptr;
            lib->left_index = nullptr;
            return lib;
        }else {
            error("not closed fucking parentheses");
            return nullptr;
        }
    }else {
        error("only support quotes(for stupid mans quotes - '\"') and parentheses");
        return nullptr;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_break() {
    if(peer().KEY == TTYPE::STRING && peer().VAL == "break") {
        advanced();
        auto node = make_unique<Node>(peer());
        node->KEY = ST_BREAK;
        node->VAL = "break";
        return node;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_continue() {
    if(peer().KEY == TTYPE::STRING && peer().VAL == "continue") {
        advanced();
        auto node = make_unique<Node>(peer());
        node->KEY = ST_CONTINUE;
        node->VAL = "continue";
        return node;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_while() {
    advanced();
    Token current = peer();
    unique_ptr<Node> then_node = nullptr;
    unique_ptr<Node> cond = nullptr;
    if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(") {
        advanced();
        cond = parse_boolea_expression();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
            advanced();
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
                advanced();
                then_node = make_unique<Node>(peer());
                then_node->KEY = ST_BLOCK;
                while(peer().KEY != TTYPE::SEPARATOR && peer().VAL != "}") {
                    unique_ptr<Node>expr = parse_statement();
                    if(expr) {
                        then_node->children.push_back(move(expr));
                    }else {
                        error("expected statement logic in ST_BLOCK");
                        return nullptr;
                    }
                    if(peer().KEY == TTYPE::END_EX) {
                        advanced();
                    }
                    if(peer().KEY == TTYPE::END) {
                        error("expected TTYPE::END in ST_BLOCK");
                        return nullptr;
                    }
                }
                advanced();
            }else {
                then_node = parse_statement();
                if(!then_node) {
                    error("expected TTYPE::UNKNOWN in term");
                    return nullptr;
                }
            }
        }else {
            error("expected sucked ')' or detected extra neurons");
            return nullptr;
        }
    }else {
        error("expected sucked '(' or detected extra chromosomes");
        return nullptr;
    }
    auto while_node = make_unique<Node>(peer());
    while_node->KEY = ST_WHILE;
    while_node->VAL = "while";
    while_node->left_index = move(cond);
    while_node->block_while = move(then_node);
    return while_node;
}
unique_ptr<Node> Parser::parse_bool() {
    Token current = peer();
    if(current.KEY == TTYPE::STRING && current.VAL == "true") {
        auto true_f = make_unique<Node>(peer());
        true_f->KEY = ST_BOOL;
        true_f->VAL = "true";
        advanced();
        return true_f;
    }
    if(current.KEY == TTYPE::STRING && current.VAL == "false") {
        auto false_f = make_unique<Node>(peer());
        false_f->KEY = ST_BOOL;
        false_f->VAL = "false";
        advanced();
        return false_f;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_if() {
    advanced();
    Token current = peer();
    unique_ptr<Node> then_node = nullptr;
    unique_ptr<Node> cond = nullptr;
    if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(") {
        advanced();
        cond = parse_boolea_expression();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
            advanced();
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
                advanced();
                then_node = make_unique<Node>(peer());
                then_node->KEY = ST_BLOCK;
                while(peer().KEY != TTYPE::SEPARATOR && peer().VAL != "}") {
                    unique_ptr<Node>expr = parse_statement();
                    if(expr) {
                        then_node->children.push_back(move(expr));
                    }else {
                        error("expected statement logic in ST_BLOCK");
                        return nullptr;
                    }
                    if(peer().KEY == TTYPE::END_EX) {
                        advanced();
                    }
                    if(peer().KEY == TTYPE::END) {
                        error("expected TTYPE::END in ST_BLOCK");
                        return nullptr;
                    }
                }
                advanced();
            }else {
                then_node = parse_statement();
                if(!then_node) {
                    error("expected TTYPE::UNKNOWN in term");
                    return nullptr;
                }
            }
        }else {
            error("expected sucked ')' or detected extra neurons");
            return nullptr;
        }
    }else {
        error("expected sucked '(' or detected extra chromosomes");
        return nullptr;
    }
    unique_ptr<Node> else_node = nullptr;
    if(peer().KEY == TTYPE::STRING && peer().VAL == "else") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
            advanced();
            else_node = make_unique<Node>(peer());
            else_node->KEY = ST_BLOCK;
            while(peer().KEY != TTYPE::SEPARATOR && peer().VAL != "}") {
                unique_ptr<Node>expr = parse_statement();
                if(expr) {
                    else_node->children.push_back(move(expr));
                }else {
                    error("expected statement logic in ST_BLOCK");
                    return nullptr;
                }
                if(peer().KEY == TTYPE::END_EX) {
                    advanced();
                }
                if(peer().KEY == TTYPE::END) {
                    error("expected '}' in ST_BLOCK");
                    return nullptr;
                }
            }
            advanced();
        }else {
            else_node = parse_statement();
            if(!else_node) {
                error("expected TTYPE::UNKNOWN in term");
                return nullptr;
            }
        }
    }
    auto if_node = make_unique<Node>(peer());
    if_node->KEY = ST_IF;
    if_node->VAL = "if";
    if_node->left_index = move(cond);
    if_node->if_index = move(then_node);
    if_node->else_index = move(else_node);
    return if_node;
}
unique_ptr<Node> Parser::parse_print() {
	advanced();
	auto print_node = make_unique<Node>(peer());
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
    	unique_ptr<Node> expr = parse_expression();
    	if(expr == nullptr) {
    		error("you didn't add the arguments fucked mudda");
    		return nullptr;
    	}
    	print_node->children.push_back(move(expr));
    	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
            advanced();
            unique_ptr<Node>t_expr = parse_expression();
            if(!t_expr) {
                error("expected expression in lmuck()");
                return nullptr;
            }
            print_node->children.push_back(move(t_expr));
        }
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();
            return print_node;
        }else {
            error("expected ')' in lmuck()");
            return nullptr;
        }
    }else {
        error("expected '(' in lmuck()");
        return nullptr;
    }
}
unique_ptr<Node> Parser::parse_stod() {
    auto lmtod = make_unique<Node>(peer());
    lmtod->KEY = ST_STOD;
    lmtod->VAL = "lmtod";
    lmtod->left_index = nullptr;
    lmtod->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            unique_ptr<Node> expr = parse_expression();
            lmtod->right_index = move(expr);
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                error("expected ')' in lmtod");
                return nullptr;
            }    
        }
        return lmtod; 
    }else {
        error("expected '(' in lmtod");
        return nullptr;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_input() {
    auto input_node = make_unique<Node>(peer());
    input_node->KEY = ST_INPUT;
    input_node->VAL = "lmout";
    input_node->left_index = nullptr;
    input_node->right_index = nullptr;
    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
            advanced();   
        }else {
            unique_ptr<Node> expr = parse_expression();
            input_node->right_index = move(expr);
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
                advanced();
            }else {
                error("expected ')' in lmout");
                return nullptr;
            }    
        }
        return input_node; 
    }else {
        error("expected '(' in lmout ");
        return nullptr;
    }
    return nullptr;
}
unique_ptr<Node> Parser::parse_statement() {
	Token current = peer();
	if (current.KEY == TTYPE::STRING && current.VAL == "lmuck") {
	    return parse_print();
	}
	else if (current.KEY == TTYPE::STRING && current.VAL == "return") {
	    return parse_return();
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
	else if (current.KEY == TTYPE::STRING && current.VAL == "for") {
	    return parse_for();
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

	    int parens = 0;
	    while(pos_expr < tokenize.size() && tokenize[pos_expr].KEY != TTYPE::END_EX) {
	        if(tokenize[pos_expr].VAL == "(" || tokenize[pos_expr].VAL == "{" || tokenize[pos_expr].VAL == "[") parens++; 
	        if(tokenize[pos_expr].VAL == ")" || tokenize[pos_expr].VAL == "}" || tokenize[pos_expr].VAL == "]") parens--; 
	        if(parens == 0 && tokenize[pos_expr].KEY == TTYPE::OPERATOR && tokenize[pos_expr].VAL == "=") {
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
unique_ptr<Node> Parser::parse_assignment() {
    vector<unique_ptr<Node>>left_elements;
    while(true) {
        unique_ptr<Node> expr = parse_expression();
        if(!expr) {
            error("unknown left_index in ASSIGNMENTATION");
            return nullptr;
        }
        if(expr->KEY != ST_VARIABLE && expr->KEY != ST_INDEX) { 
            error("unknown datatype in assignmentation"); return nullptr;
        }
        left_elements.push_back(move(expr));
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
            advanced();
            continue;
        }
        if(peer().KEY == TTYPE::OPERATOR && peer().VAL == "=") {
            break;
        }
        else {
            error("UNKNOWN::TTYPE in assignmentation");
            return nullptr; 
        }
    }
    advanced();
    if(peer().KEY == TTYPE::END) {
        error("expected left_expression in assignmentation");
        return nullptr;
    }
    vector<unique_ptr<Node>>right_elements;
    while(true) {
        unique_ptr<Node> expr = parse_expression();
        if(!expr) {
            error("expected TTYPE::OPERATOR \"=\" ");
            return nullptr;
        }
        right_elements.push_back(move(expr));
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
            advanced();
            continue;
        }else {
            break;
        }
    }
    auto assign = make_unique<Node>(peer());
    assign->KEY = ST_ASSIGNMENT;
    assign->VAL = "=";
    assign->left_index = nullptr;
    assign->children = move(left_elements);
    assign->right_children = move(right_elements);
    return assign;
}
unique_ptr<Node> Parser::parse_factor() {
	Token current = peer();
	unique_ptr<Node>left = nullptr;
    if(current.KEY == TTYPE::NUMBER) {
		auto node = make_unique<Node>(peer());
		node->KEY = ST_NUMBER;
		node->VAL = current.VAL;
		advanced();
		left = move(node);
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
	    else if(current.VAL == "lmit") {
	        advanced();
	        left = parse_wait();
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
        else if(current.VAL == "func") {
            advanced();
            left = parse_func();
        }
	    else {
	        string name = peer().VAL;
	        advanced();
    		if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "[") {
                advanced();
                unique_ptr<Node> expr = parse_expression();
                if(!expr) {
                    error("expected expression in variable index");
                    return nullptr;
                }
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                    advanced();
                }else {
                    error("expected ']' in variable index");
                    return nullptr;
                }
                auto var_node = make_unique<Node>(peer());
                var_node->KEY = ST_VARIABLE;
                var_node->VAL = name;
    		    auto index = make_unique<Node>(peer());
    		    index->KEY = ST_INDEX;
    		    index->VAL = peer().VAL;
    		    index->left_index = move(var_node);
    		    index->right_index = move(expr);
    		    left = move(index);
    		}else if (peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
    		    advanced();
    		    auto call_node = make_unique<Node>(peer());
    		    call_node->KEY = ST_CALL;
    		    call_node->VAL = move(name);
    		    while(peer().VAL != ")") {
    		        unique_ptr<Node> expr = parse_expression();
    		        if(!expr) {
    		            error("unknown expression in parentheses");
    		            return nullptr;
    		        }
    		        call_node->children.push_back(move(expr));
    		        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
    		            advanced();
    		        } else if(peer().VAL != ")") {
    		            error("expected ')' in function");
    		            return nullptr;    		            
    		        }
    		    }
    		    advanced();
    		    left = move(call_node);
    		}else {
    		    auto node = make_unique<Node>(peer());
    		    node->KEY = ST_VARIABLE;
    		    node->VAL = move(name);
    		    left = move(node);
    		}
        }
	}
    else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "[") {
        advanced();
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
            advanced();
            auto node = make_unique<Node>(peer());
            node->KEY = ST_ARRAY;
            node->children = vector<unique_ptr<Node>>();
            left = move(node);
            return left;
        }
        if(peer().KEY == TTYPE::END) {
            return nullptr;
        }
        vector<unique_ptr<Node>>elements;
        while(true) {
            unique_ptr<Node> element = parse_expression();
            if(!element) {
                error("cant parsing array,detection TTYPE::UNKNOWN in array");
                return nullptr;
            }
            elements.push_back(move(element));
            
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
                advanced();
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                    error("you shit it, the next comma in the array is forbidden");
                    return nullptr;
                }
                continue;
            }
            else if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                break;
            }else {
                error("expected stupid parenthesis in fucked ST_ARRAY");
                return nullptr;
            }
        }
        auto node = make_unique<Node>(peer());
        node->KEY = ST_ARRAY;
        node->children = move(elements);
        advanced();
        left = move(node);
    }
    else if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "{") {
        advanced();
        vector<unique_ptr<Node>>dickes;
        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "}") {
            advanced();
            auto dick = make_unique<Node>(peer());
            dick->KEY = ST_DICTIONARY;
            dick->children = vector<unique_ptr<Node>>();
            left = move(dick);
            return left;
        }
        while(true) {
            unique_ptr<Node> element = parse_expression();
            if(!element) {
                error("fucked muddaeb get out of lmnlang,expected left_element");
                return nullptr;
            }
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ":") {
                advanced();
            }else {
                error("fucked muddaeb get out of lmnlang, expected ':'");
                return nullptr; 
            }
            unique_ptr<Node> s_element = parse_expression();
            if(!s_element) {
                error("fucked muddaeb get out of lmnlang,expected right_element");
                return nullptr;                    
            }
            dickes.push_back(move(element));
            dickes.push_back(move(s_element));
            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ",") {
                advanced();
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "}") {
                    error("E: dont parsing fucked '}' sucked pidoras");
                    return nullptr;
                }
                continue;
            }
            else if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "}") {
                advanced();
                break;
            }else {
                error("EBLAN where is the parenthesis fucking?");
                return nullptr;
            }
        }
        auto dictionary = make_unique<Node>(peer());
        dictionary->KEY = ST_DICTIONARY;
        dictionary->children = move(dickes);
        left = move(dictionary);
        return left;
    }
	else if(current.KEY == TTYPE::STRING_LIT) {
		auto node = make_unique<Node>(peer());
		node->KEY = ST_STRING;
		node->VAL = current.VAL;
		advanced();
		left = move(node);
	}
	else if(current.KEY == TTYPE::END_EX) {
		advanced();
    	return nullptr;
	}
	else if(current.KEY == TTYPE::NOT) {
	    advanced();
	    unique_ptr<Node> right = parse_factor();
	    auto not_v = make_unique<Node>(peer());
	    not_v->KEY = ST_NOT;
	    not_v->VAL = "!";
	    not_v->left_index = nullptr;
	    not_v->right_index = move(right);
	    left = move(not_v);
	}
	else if(current.KEY == TTYPE::OPERATOR && current.VAL == "-") {
		advanced();
		unique_ptr<Node> right = parse_factor();
		auto node = make_unique<Node>(peer());
		node->KEY = ST_OPERATOR;
		node->VAL = "u-";
		node->left_index = nullptr;
		node->right_index = move(right);
		left = move(node);
	}
	else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "(" ) {
		advanced();
		unique_ptr<Node>inner = parse_expression();
		if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")" ) {
			advanced();
			return inner;
		}
		else {
			error("small tits on the brackets, expected ')'");
			return nullptr;
		}
	}
	else {
	    error("unknown token in parse_factor(): '" + peer().VAL + "'");
	    advanced();
		return nullptr;
	}
	if(!left) {return nullptr;}
	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "[") {
	    advanced();
	    unique_ptr<Node> expr = parse_expression();
	    if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
	        advanced();
     	    auto node = make_unique<Node>(peer());
     	    node->KEY = ST_INDEX;
     	    node->left_index = move(left);
     	    node->right_index = move(expr);
     	    left = move(node);
	    }else {
	        error("expected your brain or ']' in index");
	        return nullptr;
	    }
	}
	while(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ".") {
	    advanced();
	    if(peer().KEY == TTYPE::STRING && peer().VAL == "lmpush") {
	        advanced();
	        if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "(") {
	            advanced();
	            unique_ptr<Node> expr = parse_expression();
	            if(!expr) {
	                error("expected expression in array lmpush");
	                return nullptr;
	            }
	            if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == ")") {
	                advanced();
	                auto pusher = make_unique<Node>(peer());
	                pusher->KEY = ST_ARRAY_PUSH;
	                pusher->VAL = peer().VAL;
	                pusher->left_index = move(left);
	                pusher->right_index = move(expr);
	                left = move(pusher);
	            }else {
	                error("expected ')' in array lmpush");
	                return nullptr;
	            }
	        }
	    }
	}
	return left;
}
unique_ptr<Node> Parser::parse_boolea_expression() {
	unique_ptr<Node>left = parse_logic_expression();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::BOOLEA_OPERATOR &&  (current.VAL == "&&" || current.VAL == "||")) {
		const string current_op = current.VAL;
		advanced();
		unique_ptr<Node> right = parse_logic_expression();
		if(right == nullptr) {
		    error("missing right operand for boolea operator");
			return nullptr;
		}
		auto node = make_unique<Node>(peer());
		node->KEY = ST_BOOLEA_OPERATOR;
		node->left_index = move(left);
		node->right_index = move(right);
		node->VAL = move(current_op);
		left = move(node);
		current = peer();
	}
	return left;
}
unique_ptr<Node> Parser::parse_logic_expression() {
    unique_ptr<Node>left = parse_expression();
    if(left == nullptr) {
        return nullptr;
    }
    while(peer().KEY == TTYPE::LOGIC_OPERATOR && (peer().VAL == ">" || peer().VAL == "<" 
    || peer().VAL == "!=" || peer().VAL == "==" || peer().VAL == "<=" || peer().VAL == ">=")) {
        const string current_op = peer().VAL;
		advanced();
		unique_ptr<Node> right = parse_expression();
		if(right == nullptr) {
		    error("missing right operand for logic operator");
			return nullptr;
		}
		auto node = make_unique<Node>(peer());
		node->KEY = ST_LOGIC_OPERATOR;
		node->left_index = move(left);
		node->right_index = move(right);
		node->VAL = move(current_op);
		left = move(node);    
    }
    return left;
}
unique_ptr<Node> Parser::parse_term() {
	unique_ptr<Node> left = parse_factor();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR &&  (current.VAL == "*" || current.VAL == "/" || current.VAL == "%")) {
		const string current_op = current.VAL;
		Token op_token = peer();
		advanced();
		unique_ptr<Node> right = parse_factor();
		if(right == nullptr) {
		    error("missing right operand for logic operator");
			return nullptr;
		}
		auto node = make_unique<Node>(op_token);
		node->KEY = ST_OPERATOR;
		node->left_index = move(left);
		node->right_index = move(right);
		node->VAL = move(current_op);
		left = move(node);
		current = peer();
	}
	return left;
}
unique_ptr<Node> Parser::parse_expression() {
	unique_ptr<Node> left = parse_term();
	if(left == nullptr) {
		return nullptr;
	}
	Token current = peer();
	while(current.KEY == TTYPE::OPERATOR && (current.VAL == "+" || current.VAL == "-")) {
		const string current_op = current.VAL;
		Token op_token = peer();
		advanced();
		unique_ptr<Node> right = parse_term();
		if(right == nullptr) {
		    error("missing right operand for operator");
			return nullptr;
		}
		auto node = make_unique<Node>(op_token);
		node->KEY = ST_OPERATOR;
		node->left_index = move(left);
		node->right_index = move(right);
		node->VAL = move(current_op);
		left = move(node);
		current = peer();
	}
	return left;
}
