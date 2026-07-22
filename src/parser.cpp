/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/parser.hpp"
#include "../include/utf8_win.hpp"
#include "../include/ast.hpp"
#include "../src/manual.cpp"
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
Value Parser::evaluate(Node* node) {
	setup_utf8();
	if(!node) { return AcceptValue{}; }
	if(node->KEY == ST_NUMBER) {
		return stod(node->VAL);
	}
	else if(node->KEY == ST_NOP) {
		return AcceptValue{};
	}
	else if(node->KEY == ST_OPERATOR && node->VAL == "u-") {
		Value val = evaluate(node->right_index);
        if(holds_alternative<double>(val)) {
            return -get<double>(val);
        }else {
            cout<<"\033[1;31mE: unary minus requires a number\033[0m"<<endl;
            return ErrorValue{"E: unary minus requires a number"};
        }
	}
	else if(node->KEY == ST_BLOCK) {
	    for(Node* child : node->children) {
	        Value val = evaluate(child);
	        if(holds_alternative<ErrorValue>(val)) { return val; }
	    }
	    return AcceptValue{};
	}
	else if(node->KEY == ST_ARRAY) {
        auto arr = make_shared<ArrayValue>();
        for(Node* element : node->children) {
            Value val = (evaluate(element));
            if(holds_alternative<ErrorValue>(val)) {return val;}
            arr->elements.push_back(val);
        }
        return arr;
    }
    else if(node->KEY == ST_INDEX) {
        const Value left_val = evaluate(node->left_index);
        const Value right_val = evaluate(node->right_index);
        if(!holds_alternative<shared_ptr<ArrayValue>>(left_val)) {
           cout<<"\033[1;31mE: dont have ST_ARRAY\033[0m"<<endl;
           return ErrorValue {"E: dont have ST_ARRAY"};
        }
        if(!holds_alternative<double>(right_val)) {
            cout<<"\033[1;31mE: dont have ST_INDEX in ST_ARRAY\033[0m"<<endl;
            return ErrorValue {"E: dont have ST_INDEX in ST_ARRAY"};
        }
        auto& left = get<shared_ptr<ArrayValue>>(left_val);
        double index_double = get<double>(right_val);
        if(index_double != (int)index_double) {
            cout<<"\033[1;31mE: array index must be integer fucked kid\033[0m"<<endl;
            return ErrorValue{"E: array index must be integer fucked kid"};
        }
        int right = (int)index_double;
        if(right < 0 || (size_t)right >= left->elements.size()) {
            cout<<"\033[1;31mE: ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()\033[0m"<<endl;
            return ErrorValue {"E: ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()"};
        }
        return left->elements[right];
    }
    else if(node->KEY == ST_STRING) {
        return node->VAL;
    }
	else if(node->KEY == ST_VARIABLE) {
		const string name = node->VAL;
		const auto it = vars.find(name);
		if (it != vars.end()) {
		    return (it->second);
		}
		cout<<"\033[1;31mE: Variable not found\033[0m"<<endl;
		return ErrorValue {"E: Variable not found"};
	}
	else if(node->KEY == ST_BOOLEA_OPERATOR) { 
        Value left_val = evaluate(node->left_index);
        if(holds_alternative<ErrorValue>(left_val)) {return left_val;}
        double num = get<double>(left_val);
        bool l_bool = (num!=0.0);
        if(node->VAL =="&&") {
            if(!l_bool) {return 0.0;}
            Value right_val = evaluate(node->right_index);
            if(holds_alternative<ErrorValue>(right_val)) {return right_val; }
            double num1 = get<double>(right_val);
            bool r_bool = (num1 != 0.0);
            return r_bool ? 1.0 : 0.0;
        }
        else if(node->VAL =="||") {
            if(l_bool) { return 1.0; }
            Value right_val = evaluate(node->right_index); 
            if(holds_alternative<ErrorValue>(right_val)) {return right_val; }
            double num1 = get<double>(right_val);
            bool r_bool = (num1 != 0.0);
            return r_bool ? 1.0 : 0.0;
        }
	}
	else if(node->KEY == ST_LOGIC_OPERATOR) {
        const Value left_val = evaluate(node->left_index);
     	const Value right_val = evaluate(node->right_index);
     	if(holds_alternative<ErrorValue>(left_val)) { return left_val; }
     	if(holds_alternative<ErrorValue>(right_val)) { return right_val; }
     	if(holds_alternative<string>(left_val) && holds_alternative<string>(right_val)) {
     	    string left = get<string>(left_val);
     	    string right = get<string>(right_val);
     	    string log_op = node->VAL;
     	    if(log_op == "!=") {  return (left != right) ? 1.0 : 0.0; } 
     	    else if(log_op == "==") { return (left == right) ? 1.0 : 0.0; }
     	    return ErrorValue {"E: unknown logic operator"};   
     	}
     	if(holds_alternative<shared_ptr<ArrayValue>>(left_val) && 
     	holds_alternative<shared_ptr<ArrayValue>>(right_val) ) {
     	auto left = get<shared_ptr<ArrayValue>>(left_val);
     	auto right = get<shared_ptr<ArrayValue>>(right_val);
     	string log_op = node->VAL;
     	    if(log_op != "!=" && log_op != "==") {
     	        return ErrorValue{"E: unknown logic operator"};
     	    }
         	if(left->elements.size() != right->elements.size()) {
                return(log_op == "!=") ? 1.0 : 0.0;
         	}
         	bool equals = true;
     	    for(size_t i = 0; i < left->elements.size(); ++i) {
     	       if(left->elements.at(i) != right->elements.at(i)) {
     	            equals = false;
     	            break;
                }
            }
            if(log_op == "==") {
                return equals ? 1.0 : 0.0;
            }else {
                return equals ? 0.0 : 1.0;
            }
     	}
     	if(holds_alternative<double>(left_val) && holds_alternative<double>(right_val)) {
             double left = get<double>(left_val);
             double right = get<double>(right_val);
             string log_op = node->VAL;
             if(log_op == ">") { double mo = left > right; return mo; }
             else if(log_op == "<") { double mo = left < right; return mo; } 
             else if(log_op == ">=") { double mo = left >= right; return mo; } 
             else if(log_op == "<=") { double mo = left <= right; return mo; } 
             else if(log_op == "!=") { double mo = left != right; return mo; } 
             else if(log_op == "==") { double mo = left == right; return mo; }           
             return ErrorValue {"E: unknown logic operator"};
        }
	    if(!holds_alternative<double>(left_val) || !holds_alternative<double>(right_val)) {
	        cout<<"\033[1;31mE: logic operator return unknown TTYPE or comparison\033[0m"<<endl;
	        return ErrorValue {"E: logic operator return unknown TTYPE or comparison"};
	    }
	}
	else if(node->KEY == ST_OPERATOR) {
		const Value left_val = evaluate(node->left_index);
		const Value right_val = evaluate(node->right_index);
		if(holds_alternative<ErrorValue>(left_val)) { return left_val; }
		if(holds_alternative<ErrorValue>(right_val)) { return right_val; }
		if(!holds_alternative<double>(left_val) || !holds_alternative<double>(right_val)) {
		    cout<<"\033[1;31mE: operator requires numbers stupid people\033[0m"<<endl;
		    return ErrorValue {"E: operator requires numbers stupid people"};
		}
        double left = get<double>(left_val);
        double right = get<double>(right_val);
		string op = node->VAL;
		if(op == "+") { double mo = left + right; return mo; }
		else if(op == "-") { double mo = left - right; return mo; }
		else if(op == "/") {
			if(right != 0) {
				double mo = left / right;  return mo; 
			}
			else {
			    cout<<"\033[1;31mE: cannot be divided by fucked zero\033[0m"<<endl;
				return ErrorValue{"E: cannot be divided by fucked zero"};
			}
		}				
		else if(op == "*") { double mo = left * right;  return mo;  }
	}
	else if(node->KEY == ST_ASSIGNMENT) {
        Value result = evaluate(node->right_index);
        if(holds_alternative<ErrorValue>(result)) { return result; }
        if(!node->children.empty()) {
            for(const auto& node : node->children) {
                vars[node->VAL] = result;
            }
        }else {
            cout<<"\033[1;31mE: node->children is empty() fucking mudda\033[0m"<<endl;
            return ErrorValue {"E: node->children is empty() fucking mudda"};
        }
        return AcceptValue{};
    }
    else if(node->KEY == ST_IF) {
        Value cond_val = evaluate(node->left_index);
        if(holds_alternative<ErrorValue>(cond_val)) { return cond_val; }
        if(!holds_alternative<double>(cond_val)) { 
            cout<<"\033[1;31mE: TTYPE return value does not is equal to TTYPE::NUMBER\033[0m"<<endl;
            return ErrorValue{"E: TTYPE return value does not is equal to TTYPE::NUMBER"};
        }
        double ev_num = get<double>(cond_val);
        if(ev_num != 0.0) {
            if(node->if_index) {
                return evaluate(node->if_index);
            }
        }else {
            if(node->else_index) {
                return evaluate(node->else_index);
            }
        }
        return AcceptValue{};
    }
    else if(node->KEY == ST_STOD) {
        Value expr = evaluate(node->right_index);
        if(holds_alternative<string>(expr)) {
            try{
                string prompt = get<string>(expr);
                double convert = stod(prompt);
                return convert;
            }catch(...) {
                cout<<"\033[1;31mE: do not convert this node->right_index\033[0m"<<endl;
                return ErrorValue{"E: do not convert this node->right_index"};
            }
        }
        if(holds_alternative<double>(expr)) {return expr;} 
    }
    else if(node->KEY == ST_INPUT) {
        string prompt = "";
        if(node->right_index != nullptr) {
            Value prompt_val = evaluate(node->right_index);
            if(holds_alternative<ErrorValue>(prompt_val)) { return prompt_val; }
            if(holds_alternative<string>(prompt_val)) {
                prompt = get<string>(prompt_val);
            }
            else if(holds_alternative<double>(prompt_val)) {
                prompt = format("{:g}", get<double>(prompt_val));
            }
        }
        cout << flush;
        fflush(stdout);
        char* input_ptr = readline(prompt.c_str());
        if(input_ptr == nullptr) {
            cout<<"\033[1;31mW: input interrupted\033[0m"<<endl;
            return ErrorValue{"W: input interrupted"};
        }
        string input = input_ptr;
        free(input_ptr);
        return input; 
    }
	else if(node->KEY == ST_PRINT) {
	    for(size_t i = 0 ; i < node->children.size(); ++i) {
    	    Value val = evaluate(node->children[i]);
            if(holds_alternative<ErrorValue>(val)) { return val ; }
    	    print_array(val);
        }
        cout<<endl;
        return AcceptValue{};
	}
	else {
	    cout<<"\033[1;31mE: unknown Value Parser::evaluate() type\033[0m"<<endl;
		return ErrorValue{"E: unknown Value Parser::evaluate() type"};
	}
	return ErrorValue{"C.E: evaluate() return critical error"};
}
Node* Parser::parse_program() {
	setup_utf8();
	for(const auto& token : tokenize ) {
		if(token.KEY == TTYPE::UNKNOWN ) {
			cout<<"\033[1;31mE: Deer,my parser doesn't understand this shit\033[0m"<<endl;
			return nullptr;
		}
	}
	while(true) {
		Node* expr = parse_statement();
		if(!expr) {
			if(peer().KEY == TTYPE::END) {
				break;
			}
			cout<<"\033[1;33mE: expected expression\033[0m"<<endl;
			return nullptr;
		}
		bool s_ok = false;
		if(peer().KEY == TTYPE::END_EX) {
		    advanced();
		    s_ok = true;
		}
		else if(peer().KEY == TTYPE::END) {
		    s_ok = true;
		}
		else {
		    cout<<"\033[1;33mE: expected ';' or end of input\033[0m"<<endl;
            delete expr;
		    return nullptr;
		}
		if(s_ok) {
		    Value res = evaluate(expr);
		    if(holds_alternative<ErrorValue>(res)) {
		        if(peer().KEY == TTYPE::END) {
		            break; 
		        }
		        continue;
		    }
		    if(holds_alternative<AcceptValue>(res)) {
		        if(peer().KEY == TTYPE::END) {
		            break; 
		        }
		        continue;
		    }
		    print_array(res);
		    cout<<endl;
		}
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
        cout<<"\033[1;33mE: expected '(' after lmuck\033[0m"<<endl;
        delete print_node;
        return nullptr;
    }
}
Node* Parser::parse_stod() {
    Node* input_node = new Node();
    input_node->KEY = ST_STOD;
    input_node->VAL = "lmtod";
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
        else if(current.VAL == "lmuck") {
            left = parse_print();
        }
	    else {
    		Node* node = new Node();
    		node->KEY = ST_VARIABLE;
    		node->VAL = current.VAL;
    		advanced();
    		left = node;
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
                continue;
                if(peer().KEY == TTYPE::SEPARATOR && peer().VAL == "]") {
                    cout<<"\033[1;33mE: you shit it, the next comma in the array is forbidden\033[0m"<<endl;
                    return nullptr;
                }
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
	else if(current.KEY == TTYPE::SEPARATOR && current.VAL == "\"") {
		advanced();
		Node* inner = parse_expression();
		if(peer().KEY == TTYPE::SEPARATOR && current.VAL == "\"") {
			advanced();
			return inner;
		}
		else {
			cout<<"\033[1;33mE: small dick on the quotes, expected '\"'\033[0m"<<endl;
			return nullptr;
		}
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
		Node* right = parse_expression();
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
	while(current.KEY == TTYPE::OPERATOR &&  (current.VAL == "*" || current.VAL == "/")) {
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
