/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/interpreter.hpp"
#include "../include/utf8_win.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <fstream>
#include <format>
#include <iostream>
#if defined(__linux__) || defined(__APPLE__)
#include <readline/readline.h>
#include <unistd.h>
#endif
#include <cmath>
#include <filesystem>
#include <thread>
#include <cstdlib>
#include <cstdlib>
#include <cstdio>
#include <chrono>
void interpreter::execute_error(const string& msg,Node* node) {
    cerr<<"\033[1;31m[Run e]: "<<msg<<"\033[0m"<<endl;
    if(node && !node->VAL.empty()) {
        cerr<<" "<<node->line<<" |\n   |"<<"'"<<node->VAL<<"'"<<endl;
        cerr<<"\033[1;34m     ^--------"<<"this in col:\033[0m"<<"["<<node->col<<"]"<<endl;
    }else {
        cerr<<node->line<<"|\n  | "<<"'"<<"  "<<"'"<<endl;
        cerr<<"\033[1;34m   ^---unexpected end of input in col:\033[0m"<<"["<<node->col<<"]"<<endl;
    }
}
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
		Value val = evaluate(node->right_index.get());
		if(holds_alternative<ErrorValue>(val)) { return val; }
        if(holds_alternative<double>(val)) {
            return -get<double>(val);
        }else {
            execute_error("unary minus requires a number",node);
            return ErrorValue{"E: unary minus requires a number"};
        }
	}
	else if(node->KEY == ST_RETURN) {
	    Value val = evaluate(node->right_index.get());
	    if(holds_alternative<ErrorValue>(val)) { return val; }
	    return make_shared<ReturnFunc>(ReturnFunc{move(val)});
	}
	else if(node->KEY == ST_BLOCK) {
	    for(const auto& child : node->children) {
	        Value val = evaluate(child.get());
	        if(holds_alternative<ErrorValue>(val)) { return val; }
	        if(holds_alternative<Breaker>(val)) { return val; }
	        if(holds_alternative<Continuer>(val)) { return val; }
	        if(holds_alternative<shared_ptr<ReturnFunc>>(val)) { return val; }
	    }
	    return AcceptValue{};
	}
	else if(node->KEY == ST_FUNC) {
	    vector<string>childs;
	    for(const auto& child : node->children) {
	        if(child->KEY == ST_VARIABLE) {
	            childs.push_back(child->VAL);
	        }
	    }
	    shared_ptr<Node>body = move(node->right_index);
	    vars.back()[node->VAL] = ForFunction { 
	        childs,
	        move(body),
	        is_sys
	    };
	    return AcceptValue{};
	}
	else if(node->KEY == ST_WAIT) {
	    Value val = evaluate(node->right_index.get());
	    if(holds_alternative<ErrorValue>(val)) { return val; }
	    if(holds_alternative<string>(val)) {
	        execute_error("are you faggot? why string? here you need numbers",node);
	        return ErrorValue{"E: are you faggot? why string? here you need numbers"};
	    }
	    if(holds_alternative<bool>(val)) {
	        execute_error("are you faggot? why bool? here you need numbers",node);
	        return ErrorValue{"E: are you faggot? why bool? here you need numbers"};      
	    }
	    if(holds_alternative<shared_ptr<ArrayValue>>(val)) {
	        execute_error("are you faggot? why array? here you need numbers",node);
	        return ErrorValue{"E: are you faggot? why array? here you need numbers"};	        
	    }
	    if(holds_alternative<double>(val)) {
	        double seconds = get<double>(val);
	        this_thread::sleep_for(chrono::milliseconds(static_cast<long long>(seconds * 1000)));
	    }
	    return AcceptValue{};
	}
	else if(node->KEY == ST_CALL) {
	    const string name = node->VAL;
	    vector<Value> ev_args;
	    for(const auto& child : node->children) {
	        Value ev_arg = evaluate(child.get());
	        if(holds_alternative<ErrorValue>(ev_arg)) { return ev_arg; }
	        ev_args.push_back(move(ev_arg));
	    }
	    if(name.rfind("__builtin_",0) == 0 && !is_sys) {
	        execute_error("dont have function",node);
            return ErrorValue{};
	    };
	    if(name == "__builtin_getlast_inputinfo") {
	    #if defined(_WIN32) || defined(_WIN64)
	        LASTINPUTINFO lii;
	        lii.cbSize = sizeof(LASTINPUTINFO);
	        if(GetLastInputInfo(&lii)) {
	            DWORD current_tick = GetTickCount();
	            DWORD idle_ms = current_tick - lii.dwTime;
	            return(double)(idle_ms/1000);
	        }
	        execute_error("failed to get last input info",node);
	        return ErrorValue{};
        #else
            execute_error("this function only support windows",node);
            return ErrorValue{};
        #endif
	    }
	    if(name == "__builtin_exec") {
	        if(ev_args.size() == 1) {
	            if(!holds_alternative<string>(ev_args[0])) { 
                    execute_error("current type != STRING",node);
	                return ErrorValue{};
	            } 
	            string cmd = get<string>(ev_args[0]);
	            int code = system(cmd.c_str());
	            return (double)code;
	        }else {
 	            execute_error("expected exactly one argument",node);
      	        return ErrorValue{};
 	        }
	    }
	    if(name == "__builtin_getcwd") {
	        try {
	            return filesystem::current_path().string();
	        }
	        catch(...) {
	            execute_error("failed to get current directory",node);
	            return ErrorValue{};
	        }
	    }
	    if(name == "__builtin_read") {
	    #if defined(__linux__) || defined(__APPLE__)
	        if(ev_args.size() == 1) {
	            if(!holds_alternative<string>(ev_args[0])) { 
                    execute_error("current type != STRING",node);
	                return ErrorValue{};
	            }
	            string cmd = get<string>(ev_args[0]);
	            FILE* pipe = popen(cmd.c_str(),"r");
	            if(!pipe) {
	                execute_error("popen failed",node);
	                return ErrorValue{};
	            }	            
	            char buffer[256];
	            string result = "";
	            while(fgets(buffer,sizeof(buffer),pipe) != nullptr) {
	                result += buffer;
	            }
	            pclose(pipe);
	            return result;
	        }else {
 	            execute_error("expected exactly one argument",node);
      	        return ErrorValue{};
 	        }
	    }
	    #else
	        execute_error("this func support only for unix systems",node);
	        return ErrorValue{};
	    #endif
	    if(name == "__builtin_chdir") {
	        if(ev_args.size() == 1) {
	            if(!holds_alternative<string>(ev_args[0])) { 
                    execute_error("current type != STRING",node);
	                return ErrorValue{};
	            }
	            string path = get<string>(ev_args[0]);
	            int cd = chdir(path.c_str());
	            if(cd != 0) {
	                execute_error("chdir failed",node);
	                return ErrorValue{};
	            }
	            return (double)cd;	            
	        }else {
	            execute_error("expected exactly one argument",node);
     	        return ErrorValue{};
	        }
	    }
	    if(name == "__builtin_os") {
	        #if defined(_WIN32) || defined(_WIN64)
	            return string("windows");
	        #elif defined(__APPLE__) || defined(__MACH__)
	            return string("macos");
	        #elif defined(__linux__)
	            return string("linux");
	        #else
	            return string("unknown");
	        #endif
	    }
	    ForFunction* func_ref = nullptr;
	    bool found_func = false;
	    for(auto it = vars.rbegin(); it != vars.rend(); ++it) {
	        auto find = it->find(name);
	        if(find != it->end()) {
	            if(auto* fn = get_if<ForFunction>(&find->second)) {
	                func_ref = fn;
	                found_func = true;
	                break;
	            }
	        }
	    }
	    if(!found_func) {
	        execute_error("not found fucked function",node);
	        return ErrorValue{};
	    }
	    vector<Value>evaluated_args;
	    for(const auto& child : node->children) {
	        Value val = evaluate(child.get());
	        if(holds_alternative<ErrorValue>(val)) { return val; }
	        evaluated_args.push_back(move(val));
	    }
	    if(evaluated_args.size() != func_ref->par_names.size()) {
	        execute_error("evaluated_args.size() != func.par_names.size()",node);
	        return ErrorValue{};
	    }
	    vars.push_back({});
	    for(size_t i = 0; i < func_ref->par_names.size(); ++i) {
	        vars.back()[func_ref->par_names[i]] = move(evaluated_args[i]);
	    }
	    bool old_sys = is_sys;
	    if(func_ref->is_system) {
    	    is_sys = true;
    	}
	    Value result = evaluate(func_ref->body.get());
	    is_sys = old_sys;
	    vars.pop_back();
	    if(auto* ret = get_if<shared_ptr<ReturnFunc>>(&result)) {
	        return move((*ret)->value);
	    }
	    return result;
	}
	else if(node->KEY == ST_INCLUDE) {
        string filename = node->VAL;
        error_code ec;
        string abs_path = filesystem::weakly_canonical(filename, ec).string();
        if(ec) abs_path = filename;
        ifstream file(abs_path);
        if(!file.is_open()) {
            execute_error("file dont open",node);
            return ErrorValue{};
        }
        string code((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
        Parser parser;
        LEX lexer;
        vector<Token> tokenize = lexer.tokenize(code);
        parser.setTokens(tokenize);
        unique_ptr<Node> tree = parser.parse_program();
        if (tree != nullptr) {
            evaluate(tree.get());
        }
        return AcceptValue{};
    }
    else if(node->KEY == ST_INCLUDE_LIBS) {
        string filename = node->VAL;
        string home = getenv("HOME") ? getenv("HOME") : "";
        string full_path = home + "/LemonenLang/libs/" + node->VAL;
        if(filename.empty()) {
          cout<<"\033[1;31mE: file is empty()\033[0m"<<endl;
          return ErrorValue{};
        }
        if(!filesystem::exists(full_path) && filesystem::exists(node->VAL)) { full_path = node->VAL; }
        ifstream file(full_path);
        if(!file.is_open()) {
            execute_error("file dont open",node);
            return ErrorValue{};
        }
        string code((istreambuf_iterator<char>(file)),istreambuf_iterator<char>());
        Parser parser;
        LEX lexer;
        vector<Token> tokenize = lexer.tokenize(code);
        parser.setTokens(tokenize);
        unique_ptr<Node> tree = parser.parse_program();
        if (tree != nullptr) {
            is_sys = true;
            evaluate(tree.get());
            is_sys = false;
        }
        return AcceptValue{};	    
    }
	else if(node->KEY == ST_ARRAY_PUSH) {
	    Value left = evaluate(node->left_index.get());
	    if(holds_alternative<ErrorValue>(left)) { return left; }
	    if(!holds_alternative<shared_ptr<ArrayValue>>(left)) { 
            execute_error("left expression != <shared_ptr<ArrayValue>>",node);
	        return ErrorValue{"E: left expression != <shared_ptr<ArrayValue>>"};
	    }
	    auto arr_pusher = get<shared_ptr<ArrayValue>>(left);
	    if(!arr_pusher) {
	        execute_error("expected expression node->left_index in lmpush",node);
	        return ErrorValue{"E: expected expression node->left_index in lmpush"};
	    }
	    Value right = evaluate(node->right_index.get());
	    if(holds_alternative<ErrorValue>(left)) { return right; }
	    arr_pusher->push(right);
	    return left;   
	}
	else if(node->KEY == ST_LEN) {
	    Value val = evaluate(node->right_index.get());
	    if(holds_alternative<ErrorValue>(val)) { return val; }
	    if(holds_alternative<string>(val)) {
	        string val_expr = get<string>(val);
	        return (double)val_expr.length();
	    }
	    if(holds_alternative<double>(val)) {
	        execute_error("you can't measure the length of numbers fucked dude",node);
	        return ErrorValue{"E: you can't measure the length of numbers fucked dude"};
	    }
	    if(holds_alternative<bool>(val)) {
	        execute_error("you can't measure the length of bools fucked dude",node);
	        return ErrorValue{"E: you can't measure the length of bools fucked dude"};
	    }
	    if(holds_alternative<shared_ptr<ArrayValue>>(val)) {
	        auto expr = get<shared_ptr<ArrayValue>>(val);
	        return (double)expr->elements.size();
	    }
	    if(holds_alternative<shared_ptr<DictValue>>(val)) {
	        auto expr = get<shared_ptr<DictValue>>(val);
	        return (double)expr->dict_val.size();
	    }
	    return AcceptValue{};
	}
	else if(node->KEY == ST_TYPEOF) {
	    Value right = evaluate(node->right_index.get());
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
	        Value val = evaluate(node->left_index.get());
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
	            last_val = evaluate(node->block_while.get());
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
	    for(const auto& child : node->children) {
	        val = evaluate(child.get());
	        if(holds_alternative<ErrorValue>(val)) { return val; }
	    }
	    return val;
	}
	else if(node->KEY == ST_NOT) {
	    Value right = evaluate(node->right_index.get());
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
            execute_error("unknown TTYPE for evaluate ST_NOT '!'",node);
            return ErrorValue{"E: unknown TTYPE for evaluate ST_NOT '!'"};
        }
        return !var;
	}
	else if(node->KEY == ST_ARRAY) {
        auto arr = make_shared<ArrayValue>();
        for(const auto& element : node->children) {
            Value val = (evaluate(element.get()));
            if(holds_alternative<ErrorValue>(val)) {return val;}
            arr->elements.push_back(move(val));
        }
        return arr;
    }
    else if(node->KEY == ST_DICTIONARY) {
        auto dict = make_shared<DictValue>();
        for(size_t i = 0; i<node->children.size(); i+=2) {
            if(i+1 >= node->children.size()) {
                execute_error("expected odd match in dictionary",node);
                return ErrorValue{};
            }
            Value key = evaluate(node->children[i].get());
            Value value = evaluate(node->children[i+1].get());
            if(holds_alternative<ErrorValue>(key)) { return key; }
            if(holds_alternative<ErrorValue>(value)) { return value; }
            if(!holds_alternative<string>(key)) {
                execute_error("key is not ST_STRING,return UNKNOWN value",node);
                return ErrorValue{};
            }
            string getter = get<string>(key);
            dict->dict_val[getter] = move(value);
        }
        return dict;
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
        Value left_val = evaluate(node->left_index.get());
        Value right_val = evaluate(node->right_index.get());
        if(holds_alternative<ErrorValue>(left_val)) { return left_val; }
        if(holds_alternative<ErrorValue>(right_val)) { return right_val; }        
        if(!holds_alternative<shared_ptr<ArrayValue>>(left_val) &&
        !holds_alternative<string>(left_val) && 
        !holds_alternative<shared_ptr<DictValue>>(left_val)){
           execute_error("E: dont have ST_ARRAY",node);
           return ErrorValue {"E: dont have ST_ARRAY"};
        }
        if(holds_alternative<shared_ptr<ArrayValue>>(left_val)) {
            if(!holds_alternative<double>(right_val)) {
                execute_error("dont have ST_INDEX in ST_ARRAY",node);
                return ErrorValue {"E: dont have ST_INDEX in ST_ARRAY"};
            }
            auto& left = get<shared_ptr<ArrayValue>>(left_val);
            double index_double = get<double>(right_val);
            if(index_double != (int)index_double) {
                execute_error("array index must be integer fucked kid",node);
                return ErrorValue{"E: array index must be integer fucked kid"};
            }
            int right = (int)index_double;
            if(right < 0 || (size_t)right >= left->elements.size()) {
                execute_error("ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()",node);
                return ErrorValue {"E: ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()"};
            }
            return move(left->elements[right]);
        }
        if(holds_alternative<string>(left_val)) {
            if(!holds_alternative<double>(right_val)) {
                execute_error("dont have ST_INDEX in ST_ARRAY",node);
                return ErrorValue {"E: dont have ST_INDEX in ST_ARRAY"};
            }
            string left = get<string>(left_val);
            double index_double = get<double>(right_val);
            if(index_double != (int)index_double) {
                execute_error("string index must be integer fucked kid",node);
                return ErrorValue{"E: string index must be integer fucked kid"};
            }
            int right = (int)index_double;
            if(right < 0 || (size_t)right >= left.length()) {
                execute_error("ST_INDEX < 0 || ST_INDEX > ST_STRING.length()",node);
                return ErrorValue {"E: ST_INDEX < 0 || ST_INDEX > ST_INDEX.length()"};
            }
            return string(1,left[right]);
        }
        if(holds_alternative<shared_ptr<DictValue>>(left_val)) {
            if(!holds_alternative<string>(right_val)) {
                execute_error("dont have ST_INDEX in ST_DICTIONARY",node);
                return ErrorValue {"E: dont have ST_INDEX in ST_DICTIONARY"};
            }
            auto value = get<shared_ptr<DictValue>>(left_val);
            string key = get<string>(right_val);
            auto finder = value->dict_val.find(key);
            if(finder != value->dict_val.end()) {
                return move(finder->second);
            }else {
                execute_error("ST_INDEX returning nullptr",node);
                return ErrorValue{"E: ST_INDEX returning nullptr"};
            }
        }
        return AcceptValue{};
    }
    else if(node->KEY == ST_STRING) {
        return node->VAL;
    }
	else if(node->KEY == ST_VARIABLE) {
		const string name = node->VAL;
		for(auto it = vars.rbegin(); it != vars.rend(); ++it) {
		    auto find = it->find(name);
		    if(find != it->end()) {
		        return find->second;
		    }
		}
		execute_error("Variable not found",node);
		return ErrorValue {"E: Variable not found"};
	}
	else if(node->KEY == ST_BOOLEA_OPERATOR) { 
        Value left_val = evaluate(node->left_index.get());
        if(holds_alternative<ErrorValue>(left_val)) {return left_val;}
        double num = get<double>(left_val);
        bool l_bool = (num!=0.0);
        if(node->VAL =="&&") {
            if(!l_bool) {return 0.0;}
            Value right_val = evaluate(node->right_index.get());
            if(holds_alternative<ErrorValue>(right_val)) {return right_val; }
            double num1 = get<double>(right_val);
            bool r_bool = (num1 != 0.0);
            return r_bool ? 1.0 : 0.0;
        }
        else if(node->VAL =="||") {
            if(l_bool) { return 1.0; }
            Value right_val = evaluate(node->right_index.get()); 
            if(holds_alternative<ErrorValue>(right_val)) {return right_val; }
            double num1 = get<double>(right_val);
            bool r_bool = (num1 != 0.0);
            return r_bool ? 1.0 : 0.0;
        }
	}
	else if(node->KEY == ST_LOGIC_OPERATOR) {
        Value left_val = evaluate(node->left_index.get());
     	Value right_val = evaluate(node->right_index.get());
     	if(holds_alternative<ErrorValue>(left_val)) { return left_val; }
     	if(holds_alternative<ErrorValue>(right_val)) { return right_val; }
     	if(holds_alternative<string>(left_val) && holds_alternative<string>(right_val)) {
     	    string left = get<string>(left_val);
     	    string right = get<string>(right_val);
     	    string log_op = node->VAL;
     	    if(log_op == "!=") {  return (left != right) ? 1.0 : 0.0; } 
     	    else if(log_op == "==") { return (left == right) ? 1.0 : 0.0; }
     	    execute_error("unknown logic operator",node);
     	    return ErrorValue {"E: unknown logic operator"};   
     	}
     	if(holds_alternative<bool>(left_val) && holds_alternative<bool>(right_val)) {
     	    bool left = get<bool>(left_val);
     	    bool right = get<bool>(right_val);
     	    string log_op = node->VAL;
     	    if(log_op == "!=") { return left != right; }
     	    else if(log_op == "==") { return left == right; }
     	    execute_error("unknown logic operator",node);
     	    return ErrorValue {"E: unknown logic operator"};
     	}
     	if(holds_alternative<shared_ptr<ArrayValue>>(left_val) && 
     	holds_alternative<shared_ptr<ArrayValue>>(right_val) ) {
     	auto left = get<shared_ptr<ArrayValue>>(left_val);
     	auto right = get<shared_ptr<ArrayValue>>(right_val);
     	string log_op = node->VAL;
     	    if(log_op != "!=" && log_op != "==") {
     	        execute_error("unknown logic operator",node);
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
             execute_error("unknown logic operator",node);           
             return ErrorValue {"E: unknown logic operator"};
        }
	    if(!holds_alternative<double>(left_val) || !holds_alternative<double>(right_val)) {
	        execute_error("logic operator return unknown TTYPE or comparison",node);
	        return ErrorValue {"E: logic operator return unknown TTYPE or comparison"};
	    }
	}
	else if(node->KEY == ST_OPERATOR) {
		Value left_val = evaluate(node->left_index.get());
		Value right_val = evaluate(node->right_index.get());
		if(holds_alternative<ErrorValue>(left_val)) { return left_val; }
		if(holds_alternative<ErrorValue>(right_val)) { return right_val; }
		if(!holds_alternative<double>(left_val) || !holds_alternative<double>(right_val)) {
		    execute_error("operator requires numbers stupid people",node);
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
			    execute_error("cannot be divided by fucked zero",node);
				return ErrorValue{"E: cannot be divided by fucked zero"};
			}
		}				
		else if(op == "*") { double mo = left * right;  return mo;  }
		else if(op == "%") { double mo = fmod(left,right); return mo; }
	}
	else if(node->KEY == ST_ASSIGNMENT) {
        if(node->right_children.empty()) {
            execute_error("right_elements is empty(), open your eyes asshole",node);
            return ErrorValue{};
        }
        vector<Value>right_values;
        for(const auto& child : node->right_children) {
            Value res = evaluate(child.get());
            if(holds_alternative<ErrorValue>(res)) { return res; };
            right_values.push_back(move(res));
        }
        if(node->children.size() == 1 && right_values.size() > 1) {
            string res = "";
            for(const auto& chil : node->right_children) {
                Value ev = evaluate(chil.get());
                if(holds_alternative<ErrorValue>(ev)) { return ev; }
                if(holds_alternative<string>(ev)) {
                    res += get<string>(ev);
                }
                if(holds_alternative<double>(ev)) { 
                    double num = get<double>(ev);
                    if(fmod(num, 1.0) == 0.0) {
                        res += to_string(static_cast<long long>(num));
                    } else {
                        res += to_string(num);
                    }
                }
                if(holds_alternative<bool>(ev)) {
                    res += (get<bool>(ev) ? "true" : "false");
                }
                if(holds_alternative<shared_ptr<ArrayValue>>(ev)) {
                    string arr = "[";
                    auto getter = get<shared_ptr<ArrayValue>>(ev);
                    for(unsigned i = 0; i < getter->elements.size(); ++i) {
                        Value item = move(getter->elements[i]);
                        if(holds_alternative<string>(item)) {
                            arr += get<string>(item);
                        }
                        if(holds_alternative<double>(item)) {
                            double num = get<double>(item);
                            if(fmod(num, 1.0) == 0.0) {
                                arr += to_string(static_cast<long long>(num));
                            } else {
                                arr += to_string(num);
                            }
                        }
                        if(holds_alternative<bool>(item)) {
                            arr += (get<bool>(item) ? "true" : "false");
                        }
                        if(i + 1 <getter->elements.size()) {
                            arr += ", ";
                        }
                    }
                    arr += "]";
                    res += arr;
                }
            }
            const string result = node->children[0]->VAL;
            vars.back()[result] = res;
        }
        else if(node->children.size() == right_values.size()) {
            for(unsigned i = 0; i < node->children.size(); i++) {
                if(node->children[i]->KEY == ST_VARIABLE) {
                    string name = node->children[i]->VAL;
                    vars.back()[name] = move(right_values[i]);
                }
                if(node->children[i]->KEY == ST_INDEX) {
                    Value target = evaluate(node->children[i]->left_index.get());
                    Value index = evaluate(node->children[i]->right_index.get());
                    if(holds_alternative<ErrorValue>(target)) return target;
                    if(holds_alternative<ErrorValue>(index)) return index;
                    if(holds_alternative<shared_ptr<ArrayValue>>(target)) {
                        auto getter = get<shared_ptr<ArrayValue>>(target);
                        double idx_d = get<double>(index);
                        if(idx_d < 0) {
                            execute_error("index in array < 0",node);
                            return ErrorValue{};
                        }
                        size_t idx = static_cast<size_t>(idx_d);
                        if(idx < getter->elements.size()) {
                            getter->elements[idx] = move(right_values[i]);   
                        }else {
                            execute_error("ST_INDEX > ARRAY.SIZE()",node);
                            return ErrorValue{};
                        }
                    }else if(holds_alternative<shared_ptr<DictValue>>(target)) {
                        if(!holds_alternative<string>(index)) {
                            execute_error("index is not string,return unknown value",node);
                            return ErrorValue{};
                        }
                        auto get_target = get<shared_ptr<DictValue>>(target);
                        string get_index = get<string>(index);
                        get_target->dict_val[get_index] = move(right_values[i]);
                    }else {
                        execute_error("this is not correct type for ST_INDEX",node);
                        return ErrorValue{};
                    }
                }
            }
        }
        else if(node->children.size() > 1 && right_values.size() == 1) {
            Value expr = move(right_values[0]);
            for(const auto& child : node->children) {
                const string name = child->VAL;
                vars.back()[name] = move(expr);
            }
        }
        else {
            execute_error("node->children is empty() fucking mudda",node);
            return ErrorValue {"E: node->children is empty() fucking mudda"};
        }
        return AcceptValue{};
    }
    else if(node->KEY == ST_IF) {
        Value cond_val = evaluate(node->left_index.get());
        if(holds_alternative<ErrorValue>(cond_val)) { return cond_val; }
        bool is_condition = true;
        if(holds_alternative<double>(cond_val)) {
            is_condition = (get<double>(cond_val) != 0.0 );
        }
        else if(holds_alternative<bool>(cond_val)) {
            is_condition = get<bool>(cond_val);
        }
        else {    
            execute_error("TTYPE return value does not is equal to TTYPE::NUMBER or bool",node);
            return ErrorValue{"E: TTYPE return value does not is equal to TTYPE::NUMBER or bool"};
        }
        if(is_condition) {
            if(node->if_index) {
                return evaluate(node->if_index.get());
            }
        }else {
            if(node->else_index) {
                return evaluate(node->else_index.get());
            }
        }
        return AcceptValue{};
    }
    else if(node->KEY == ST_STOD) {
        Value expr = evaluate(node->right_index.get());
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
            Value prompt_val = evaluate(node->right_index.get());
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
        #if defined(_WIN32) || defined(_WIN64)
        if(!getline(cin,input)) {
            execute_error("lmout getline for noobs returning feces",node);
            return ErrorValue{};
        }
        #else
        char* input_ptr = readline(prompt.c_str());
        if(input_ptr == nullptr) {
            execute_error("input interrupted",node);
            return ErrorValue{"E: input interrupted"};
        }
        string input = input_ptr;
        free(input_ptr);
        #endif
        return input; 
    }
	else if(node->KEY == ST_PRINT) {
	    for(size_t i = 0 ; i < node->children.size(); ++i) {
    	    Value val = evaluate(node->children[i].get());
            if(holds_alternative<ErrorValue>(val)) { return val ; }
    	    print_array(val);
        }
        cout<<endl;
        return AcceptValue{};
	}
	else {
	    execute_error("unknown Value Parser::evaluate() type",node);
		return ErrorValue{"E: unknown Value Parser::evaluate() type"};
	} 
	return ErrorValue{"C.E: evaluate() return critical error"};
}
