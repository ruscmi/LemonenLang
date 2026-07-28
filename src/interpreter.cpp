/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/interpreter.hpp"
#include "../include/utf8_win.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <fstream>
#include <format>
#include <iostream>
#include <readline/readline.h>
#include <cmath>
Value interpreter::evaluate(Node* node) {
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
	        if(holds_alternative<Breaker>(val)) { return val; }
	        if(holds_alternative<Continuer>(val)) { return val; }
	    }
	    return AcceptValue{};
	}
	else if(node->KEY == ST_INCLUDE) {
	    string filename = node->VAL;
	    ifstream file(filename);
	    string code((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
	    Parser parser;
	    LEX lexer;
	    vector<Token> tokenize = lexer.tokenize(code);
	    parser.setTokens(tokenize);
	    Node* tree = parser.parse_program();
		if (tree != nullptr) {
			evaluate(tree);
		}
		return AcceptValue{};
	}
	else if(node->KEY == ST_INCLUDE_LIBS) {
	    string filename = node->VAL;
	    string full_path = "libs/" + node->VAL;
	    ifstream file(full_path);
	    string code((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
	    Parser parser;
	    LEX lexer;
	    vector<Token> tokenize = lexer.tokenize(code);
	    parser.setTokens(tokenize);
	    Node* tree = parser.parse_program();
		if (tree != nullptr) {
			evaluate(tree);
		}
		return AcceptValue{};	    
	}
	else if(node->KEY == ST_ARRAY_PUSH) {
	    Value left = evaluate(node->left_index);
	    if(holds_alternative<ErrorValue>(left)) { return left; }
	    if(!holds_alternative<shared_ptr<ArrayValue>>(left)) { 
            cout<<"\033[1;31mE: left expression != <shared_ptr<ArrayValue>>\033[0m"<<endl;
	        return ErrorValue{"E: left expression != <shared_ptr<ArrayValue>>"};
	    }
	    auto arr_pusher = get<shared_ptr<ArrayValue>>(left);
	    if(!arr_pusher) {
	        cout<<"\033[1;31mE: expected expression node->left_index in lmpush\033[0m"<<endl;
	        return ErrorValue{"E: expected expression node->left_index in lmpush"};
	    }
	    Value right = evaluate(node->right_index);
	    if(holds_alternative<ErrorValue>(left)) { return right; }
	    arr_pusher->push(right);
	    return left;   
	}
	else if(node->KEY == ST_LEN) {
	    Value val = evaluate(node->right_index);
	    if(holds_alternative<ErrorValue>(val)) { return val; }
	    if(holds_alternative<string>(val)) {
	        string val_expr = get<string>(val);
	        return (double)val_expr.length();
	    }
	    if(holds_alternative<double>(val)) {
	        cout<<"\033[1;31mE: you can't measure the length of numbers fucked dude\033[0m"<<endl;
	        return ErrorValue{"E: you can't measure the length of numbers fucked dude"};
	    }
	    if(holds_alternative<bool>(val)) {
	        cout<<"\033[1;31mE: you can't measure the length of bools fucked dude\033[0m"<<endl;
	        return ErrorValue{"E: you can't measure the length of bools fucked dude"};
	    }
	    if(holds_alternative<shared_ptr<ArrayValue>>(val)) {
	        auto expr = get<shared_ptr<ArrayValue>>(val);
	        return (double)expr->elements.size();
	    }
	    return AcceptValue{};
	}
	else if(node->KEY == ST_TYPEOF) {
	    Value right = evaluate(node->right_index);
        if(holds_alternative<ErrorValue>(right)) { return string("ERR"); }
	    if(holds_alternative<double>(right)) {
	        return string("NUM");
	    }
	    if(holds_alternative<string>(right)) {
	        return string("STR");
	    }
	    if(holds_alternative<bool>(right)) {
	        return string("BOOL");
	    }
	    if(holds_alternative<shared_ptr<ArrayValue>>(right)) {
	        return string("ARR");
	    }
	    return AcceptValue{};
	}
	else if(node->KEY == ST_BREAK) {
	    return Breaker{};
	}
	else if(node->KEY == ST_CONTINUE) {
	    return Continuer{};
	}
	else if(node->KEY == ST_WHILE) {
	    Value last_val;
	    while(true) {
	        Value val = evaluate(node->left_index);
	        if(holds_alternative<ErrorValue>(val)) { return val; }
	        if(holds_alternative<bool>(val)) {
	            if(!get<bool>(val)) {
	                break;
	            }
	        }
	        if(holds_alternative<double>(val)) {
	            if(get<double>(val) == 0.0) {
	                break;
	            }
	        }
	        if(node->block_while) {
	            last_val = evaluate(node->block_while);
	            if(holds_alternative<ErrorValue>(last_val)) { return last_val; }
	            if(holds_alternative<Breaker>(last_val)) { last_val = AcceptValue{}; break; }
	            if(holds_alternative<Continuer>(last_val)) { last_val = AcceptValue{}; continue; }	            
	        }else {
	            break;
	        }
	    }
	    return last_val;
	}
	else if(node->KEY == ST_PROGRAM) {
	    Value val = AcceptValue{};
	    for(auto* child : node->children) {
	        val = evaluate(child);
	        if(holds_alternative<ErrorValue>(val)) { return val; }
	    }
	    return val;
	}
	else if(node->KEY == ST_NOT) {
	    Value right = evaluate(node->right_index);
	    if(holds_alternative<ErrorValue>(right)) { return right; }
	    bool var = true;
        if(holds_alternative<bool>(right)) {
            var = get<bool>(right);
        }
        else if(holds_alternative<double>(right)) {
            var = (get<double>(right) != 0.0);
        }
        else if(holds_alternative<string>(right)) {
            var = !get<string>(right).empty();
        }
        else if(holds_alternative<shared_ptr<ArrayValue>>(right)) {
            auto arr = get<shared_ptr<ArrayValue>>(right);
            var = arr->elements.empty();
        }else {
            cout<<"\033[1;31mE: unknown TTYPE for evaluate ST_NOT '!'\033[0m"<<endl;
            return ErrorValue{"E: unknown TTYPE for evaluate ST_NOT '!'"};
        }
        return !var;
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
    else if(node->KEY == ST_BOOL) {
        if(node->VAL == "true") {
            Value val = true;
            if(holds_alternative<ErrorValue>(val)) { return val; }
            return val;
        }
        if(node->VAL == "false") {
            Value val = false;
            if(holds_alternative<ErrorValue>(val)) { return val; }
            return val;
        }
        return AcceptValue{};
    }
    else if(node->KEY == ST_INDEX) {
        const Value left_val = evaluate(node->left_index);
        const Value right_val = evaluate(node->right_index);
        if(!holds_alternative<shared_ptr<ArrayValue>>(left_val) &&
        !holds_alternative<string>(left_val)) {
           cout<<"\033[1;31mE: dont have ST_ARRAY\033[0m"<<endl;
           return ErrorValue {"E: dont have ST_ARRAY"};
        }
        if(!holds_alternative<double>(right_val)) {
            cout<<"\033[1;31mE: dont have ST_INDEX in ST_ARRAY\033[0m"<<endl;
            return ErrorValue {"E: dont have ST_INDEX in ST_ARRAY"};
        }
        if(holds_alternative<shared_ptr<ArrayValue>>(left_val)) {
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
        if(holds_alternative<string>(left_val)) {
            string left = get<string>(left_val);
            double index_double = get<double>(right_val);
            if(index_double != (int)index_double) {
                cout<<"\033[1;31mE: string index must be integer fucked kid\033[0m"<<endl;
                return ErrorValue{"E: string index must be integer fucked kid"};
            }
            int right = (int)index_double;
            if(right < 0 || (size_t)right >= left.length()) {
                cout<<"\033[1;31mE: ST_INDEX < 0 || ST_INDEX > ST_STRING.length()\033[0m"<<endl;
                return ErrorValue {"E: ST_INDEX < 0 || ST_INDEX > ST_INDEX.length()"};
            }
            return string(1,left[right]);
        }
        return AcceptValue{};
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
     	if(holds_alternative<bool>(left_val) && holds_alternative<bool>(right_val)) {
     	    bool left = get<bool>(left_val);
     	    bool right = get<bool>(right_val);
     	    string log_op = node->VAL;
     	    if(log_op == "!=") { return left != right; }
     	    else if(log_op == "==") { return left == right; }
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
		else if(op == "%") { double mo = fmod(left,right); return mo; }
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
        bool is_condition = true;
        if(holds_alternative<double>(cond_val)) {
            is_condition = (get<double>(cond_val) != 0.0 );
        }
        else if(holds_alternative<bool>(cond_val)) {
            is_condition = get<bool>(cond_val);
        }
        else {    
            cout<<"\033[1;31mE: TTYPE return value does not is equal to TTYPE::NUMBER or bool\033[0m"<<endl;
            return ErrorValue{"E: TTYPE return value does not is equal to TTYPE::NUMBER or bool"};
        }
        if(is_condition) {
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
                if(prompt.empty()) {
                    return expr;
                }
                double convert = stod(prompt);
                return convert;
            }catch(...) {
                return expr;
            }
        }
        if(holds_alternative<double>(expr)) {return expr;} 
        return expr;
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
