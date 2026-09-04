/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#include "../include/interpreter.hpp"
#include "../include/lexer.hpp"
#include "../include/parser.hpp"
#include <fstream>
#include <format>
#include <iostream>
#if defined(__linux__) || defined(__APPLE__)
#include <linux/input-event-codes.h>
#include <readline/readline.h>
#include <fcntl.h>
#include <linux/input.h>
#include <termios.h>
#endif
#include <cmath>
#include <filesystem>
#include <thread>
#include <cstdlib>
#include <cstdio>
#include <chrono>
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif
using namespace std;
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
	else if(node->KEY == ST_EMPTY) {
		Value left = evaluate(node->left_index.get());
		if(holds_alternative<ErrorValue>(left)) {
			return left;
		}
		if(holds_alternative<bool>(left)) {
			execute_error("boolean value dont have lmpty method",node);
			return ErrorValue{};
		}
		if(holds_alternative<string>(left)) {
			execute_error("string value dont have lmpty method",node);
			return ErrorValue{};
		}
		if(holds_alternative<double>(left)) {
			execute_error("double value dont have lmpty method",node);
			return ErrorValue{};
		}
		if(holds_alternative<shared_ptr<ArrayValue>>(left)) {
			auto arr = get<shared_ptr<ArrayValue>>(left);
			if(!arr) {
				execute_error("dont find array lmpty returned ErrorValue",node);
				return ErrorValue{};
			}
			return Value(arr->elements.empty() ? "yes" : "no" );
		}
		if(holds_alternative<shared_ptr<DictValue>>(left)) {
			auto dict = get<shared_ptr<DictValue>>(left);
			if(!dict) {
				execute_error("dont find dictionary lmpty returned ErrorValue",node);
				return ErrorValue{};
			}
			return Value(dict->dict_val.empty() ? "yes" : "no" );
		}
		return AcceptValue{};
	}
	else if(node->KEY == ST_FOR) {
	    if(node->children.size() < 4) {
            execute_error("for_node arguments return null value",node);
	        return ErrorValue{};
	    }
	    Value init_res = evaluate(node->children[0].get());
	    if(holds_alternative<ErrorValue>(init_res)) { return init_res; }
	    while(true) {
    	    bool is_true = false;
    	    Value cond_val = evaluate(node->children[1].get());
    	    if(holds_alternative<ErrorValue>(cond_val)) { return cond_val; }
    	    if(holds_alternative<double>(cond_val)) {
    	        double num = get<double>(cond_val);
    	        is_true = (num != 0.0);
    	    }
    	    else if(holds_alternative<bool>(cond_val)) {
    	        bool boolea = get<bool>(cond_val);
    	        is_true = boolea;
    	    }
    	    else {
    	        execute_error("unknown argument in for->arguments[1]",node);
    	        return ErrorValue{};
    	    }
    	    if(!is_true) {
    	        break;
    	    }
    	    Value body_res = evaluate(node->children[3].get());
    	    if(holds_alternative<ErrorValue>(body_res)) { return body_res; }
    	    Value step = evaluate(node->children[2].get());
    	    if(holds_alternative<ErrorValue>(step)) { return step; }
	    }
	    return AcceptValue{};
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
	    /* TIME __builtin for devs */
	    if(name == "__builtin_wait") {
	    	if(ev_args.size() < 1) {
	    		execute_error("less than two arguments passed",node);
	    		return ErrorValue{};
	    	}
		    if(holds_alternative<ErrorValue>(ev_args[0])) { return ev_args[0]; }
		    if(holds_alternative<string>(ev_args[0])) {
		        execute_error("why string? here you need numbers",node);
		        return ErrorValue{"why string? here you need numbers"};
		    }
		    if(holds_alternative<bool>(ev_args[0])) {
		        execute_error("why bool? here you need numbers",node);
		        return ErrorValue{"why bool? here you need numbers"};      
		    }
		    if(holds_alternative<shared_ptr<ArrayValue>>(ev_args[0])) {
		        execute_error("why array? here you need numbers",node);
		        return ErrorValue{"why array? here you need numbers"};	        
		    }
		    if(holds_alternative<double>(ev_args[0])) {
		        double seconds = get<double>(ev_args[0]);
		        this_thread::sleep_for(chrono::milliseconds(static_cast<long long>(seconds * 1000)));
		    }
		    return AcceptValue{};
	    }
	    if(name == "__builtin_gettime") {
	    	if(ev_args.size() == 0) {
		    	auto nowtime = chrono::system_clock::now();
		    	std::time_t currenttm = chrono::system_clock::to_time_t(nowtime);
		    	cout<<ctime(&currenttm);
	    	}
	    	return AcceptValue{};
	    }
	    if(name == "__builtin_spectime") {
			if(ev_args.size() == 0) {
				auto nowtime = chrono::system_clock::now();
				auto seconds = chrono::duration_cast<chrono::seconds>(nowtime.time_since_epoch()).count();
				return Value(static_cast<double>(seconds));
			}
	    }
	    if(name == "__builtin_consttime") {
	    	if(ev_args.size() == 1) {
				double raw_sec = get<double>(ev_args[0]);
				time_t tm_sec = static_cast<time_t>(raw_sec);
				string time_str = ctime(&tm_sec);
				if(!time_str.empty() && time_str.back() == '\n') {
					time_str.pop_back();
				}
				return Value(time_str);
	    	}
	    }
	    if(name == "__builtin_zonetime") {
	    	if(ev_args.empty()) {
	    		execute_error("less args is empty",node);
	    		return ErrorValue{};
	    	}
	    	if(holds_alternative<ErrorValue>(ev_args[0])) { return ev_args[1]; }
	    	if(ev_args.size() != 1) {
	    		execute_error("less than two args",node);
	    		return ErrorValue{};
	    	}
	    	if(!holds_alternative<string>(ev_args[0])) {
	    		execute_error("The arguments passed in should look like this: continent/city",node);
	    		return ErrorValue{};
	    	}
	    	try {
		    	auto now = chrono::system_clock::now();
		    	string get_st = get<string>(ev_args[0]);
		    	auto local_zone = chrono::locate_zone(get_st);
		    	auto time_zone = local_zone->to_local(now);
		    	cout<<std::format("{:%Y-%m-%d %H:%M:%S}\n",time_zone);
	    	}
	    	catch(const runtime_error& error) {
	    		execute_error("unknown timezone name",node);
	    		return ErrorValue{};
	    	}
	    	return AcceptValue{};
	    }
	    /* SYSTEM __builtin for devs */
	    if(name == "__builtin_keypressed") {
		    if(ev_args.size() != 1) {
		    	execute_error("expected arguments < 1",node);
		    	return ErrorValue{};
		    }
		    if(!holds_alternative<string>(ev_args[0])) {
		    	execute_error("zero arg is not string",node);
		    	return ErrorValue{};
		    }
	    	#if defined(__linux__)
	    		static const unordered_map<string,int>KEY_MAP = {
	    			{"up",KEY_UP},{"down",KEY_DOWN},
	    			{"left",KEY_LEFT},{"right",KEY_RIGHT},
	    			{"a",KEY_A},{"b",KEY_B},{"c",KEY_C},
	    			{"d",KEY_D},{"s",KEY_S},{"v",KEY_V},
	    			{"q",KEY_Q},{"e",KEY_E},{"r",KEY_R},
	    			{"x",KEY_X},{"n",KEY_N},{"h",KEY_H},
	    			{"y",KEY_Y},{"w",KEY_W},{"j",KEY_J},
	    			{"z",KEY_Z},{"m",KEY_M},{"l",KEY_L},
	    			{"p",KEY_P},{"f",KEY_F},{"k",KEY_K},
	    			{"t",KEY_T},{"g",KEY_G},{"o",KEY_O},
	    			{"u",KEY_U},{"i",KEY_I},{"space",KEY_SPACE},
	    			{"right_shift",KEY_RIGHTSHIFT},{"left_shift",KEY_LEFTSHIFT},
	    			{"enter",KEY_ENTER},{"backspace",KEY_BACKSPACE},
	    			{"tab",KEY_TAB},{"caps",KEY_CAPSLOCK},{"left_ctrl",KEY_LEFTCTRL},
	    			{"right_ctrl",KEY_RIGHTCTRL},{"left_alt",KEY_LEFTALT},{"right_alt",KEY_RIGHTALT},
	    			{"1",KEY_1},{"2",KEY_2},{"3",KEY_3},{"4",KEY_4},
	    			{"5",KEY_5},{"6",KEY_6},{"7",KEY_7},{"8",KEY_8},{"9",KEY_9},{"0",KEY_0},
	    			{"-",KEY_MINUS},{"_",KEY_MINUS},{"+",KEY_EQUAL},{"=",KEY_EQUAL},{"[",KEY_LEFTBRACE},
	    			{"]",KEY_RIGHTBRACE},{"<",KEY_COMMA},{">",KEY_DOT},{",",KEY_COMMA},{".",KEY_DOT},
	    			{"/",KEY_SLASH},{"\\",KEY_BACKSLASH},{"`",KEY_GRAVE},{"ctrl",KEY_LEFTCTRL},{"shift",KEY_LEFTSHIFT},
	    			{"alt",KEY_LEFTALT},{";",KEY_SEMICOLON},{":",KEY_SEMICOLON},{"'",KEY_APOSTROPHE},{"\"",KEY_APOSTROPHE},
	    			{"~",KEY_GRAVE},{"?",KEY_SLASH},{"esc",KEY_ESC},{"delete",KEY_DELETE},{"insert",KEY_INSERT},{"home",KEY_HOME},
	    			{"end",KEY_END},{"pageup",KEY_PAGEUP},{"pagedown",KEY_PAGEDOWN},{"f1",KEY_F1},{"f2",KEY_F2},{"f3",KEY_F3},{"f4",KEY_F4},
	    			{"f5",KEY_F5},{"f6",KEY_F6},{"f7",KEY_F7},{"f8",KEY_F8},{"f9",KEY_F9},{"f10",KEY_F10},{"f11",KEY_F11},{"f12",KEY_F12},
	    			{"win",KEY_LEFTMETA},{"super",KEY_LEFTMETA},{"meta",KEY_LEFTMETA},{"right_win",KEY_RIGHTMETA},{"right_super",KEY_RIGHTMETA},
	    			{"num_0",KEY_KP0},{"num_1",KEY_KP1},{"num_2",KEY_KP2},{"num_3",KEY_KP3},{"num_4",KEY_KP4},{"num_5",KEY_KP5},{"num_6",KEY_KP6},
	    			{"num_7",KEY_KP7},{"num_8",KEY_KP8},{"num_9",KEY_KP9},{"num_enter",KEY_KPENTER},{"num_plus",KEY_KPPLUS},{"num_minus",KEY_KPMINUS},
	    			{"num_mul",KEY_KPASTERISK},{"num_div",KEY_KPSLASH},{"num_dot",KEY_KPDOT},{"num_lock",KEY_NUMLOCK}
	    		};
	    	#elif defined(_WIN32) || defined(_WIN64)
	    		static const unordered_map<string,int>KEY_MAP = {
	    			{"up",VK_UP},{"down",VK_DOWN},
	    			{"left",VK_LEFT},{"right",VK_RIGHT},
	    			{"a",'A'},{"b",'B'},{"c",'C'},{"d",'D'},
	    			{"q",'Q'},{"w",'W'},{"e",'E'},{"r",'R'},
	    			{"t",'T'},{"y",'Y'},{"u",'U'},{"i",'I'},
	    			{"p",'P'},{"o",'O'},{"s",'S'},{"f",'F'},
	    			{"g",'G'},{"h",'H'},{"j",'J'},{"k",'K'},
	    			{"l",'L'},{"z",'Z'},{"x",'X'},{"v",'V'},
	    			{"n",'N'},{"m",'M'},{"1",'1'},{"2",'2'},
	    			{"3",'3'},{"4",'4'},{"5",'5'},{"6",'6'},
	    			{"7",'7'},{"8",'8'},{"9",'9'},{"0",'0'},
	    			{"space",VK_SPACE},{"enter",VK_RETURN},
	    			{"backspace",VK_BACK},{"tab",VK_TAB},
	    			{"caps",VK_CAPITAL},{"ctrl",VK_CONTROL},
	    			{"left_ctrl",VK_LCONTROL},{"right_ctrl",VK_RCONTROL},
	    			{"shift",VK_SHIFT},{"right_shift",VK_RSHIFT},{"left_shift",VK_LSHIFT},
	    			{"alt",VK_MENU},{"left_alt",VK_LMENU},{"right_alt",VK_RMENU},
	    			{"win",VK_LWIN},{"super",VK_LWIN},{"right_win",VK_RWIN},
	  				{"-",VK_OEM_MINUS},{"_",VK_OEM_MINUS},{"+",VK_OEM_PLUS},{"=",VK_OEM_PLUS},
	  				{"[",VK_OEM_4},{"]",VK_OEM_6},{",",VK_OEM_COMMA},{"<",VK_OEM_COMMA},{".",VK_OEM_PERIOD},
	  				{">",VK_OEM_PERIOD},{"/",VK_OEM_2},{"?",VK_OEM_2},{"\\",VK_OEM_5},{"`",VK_OEM_3},{"~",VK_OEM_3},
	  				{";",VK_OEM_1},{":",VK_OEM_1},{"'",VK_OEM_7},{"\"",VK_OEM_7},
	  				{"f1",VK_F1},{"f2",VK_F2},{"f3",VK_F3},{"f4",VK_F4},{"f5",VK_F5},{"f6",VK_F6},{"f7",VK_F7},{"f8",VK_F8},
	  				{"f9",VK_F9},{"f10",VK_F10},{"f11",VK_F11},{"f12",VK_F12},{"num_0",VK_NUMPAD0},{"num_1",VK_NUMPAD1},
	  				{"num_2",VK_NUMPAD2},{"num_3",VK_NUMPAD3},{"num_4",VK_NUMPAD4},{"num_5",VK_NUMPAD5},{"num_6",VK_NUMPAD6},
	  				{"num_7",VK_NUMPAD7},{"num_8",VK_NUMPAD8},{"num_9",VK_NUMPAD9},{"num_enter",VK_RETURN},{"num_plus",VK_ADD},
	  				{"num_minus",VK_SUBTRACT},{"num_div",VK_DIVIDE},{"num_dot",VK_DECIMAL},{"num_lock",VK_NUMLOCK},{"num_mul",VK_MULTIPLY},
	  				{"esc",VK_ESCAPE},{"delete",VK_DELETE},{"home",VK_HOME},{"pageup",VK_PRIOR},{"pagedown",VK_NEXT},{"insert",VK_INSERT},{"end",VK_END}
	    		};
	    	#else
	    		#error "expected unknown OS"
	    	#endif
	    	string key_name = get<string>(ev_args[0]);
    		auto it = KEY_MAP.find(key_name);
    		if(it == KEY_MAP.end()) {
    			return (double)0;
    		}
    		int ac_code = it->second;
    		#if defined(_WIN32) || defined(_WIN64)
    			SHORT state = GetAsyncKeyState(ac_code);
    			bool is_pressed = (state & 0x8000) != 0;
    			return (double)(is_pressed ? 1 : 0);
    		#elif defined(__linux__)
    		    static unordered_map<int, bool> glob_k_stat;
    		    static bool initialized = false;
    		    static vector<int> fds;
    		    if (!initialized) {
    		        for(int i = 0; i < 32; ++i) {
    		            string dev_path = "/dev/input/event" + to_string(i);
    		            int fd = open(dev_path.c_str(), O_RDONLY | O_NONBLOCK);
    		            if (fd >= 0) {
    		                fds.push_back(fd);
    		            }
    		        }
    		        initialized = true;
    		    }
    		    struct input_event ev;
    		    for (int fd : fds) {
    		        while(read(fd, &ev, sizeof(struct input_event)) > 0) {
    		            if(ev.type == EV_KEY) {
    		                if(ev.value == 1) {
    		                    glob_k_stat[ev.code] = true;
    		                } else if(ev.value == 0) {
    		                    glob_k_stat[ev.code] = false;
    		                }
    		            }
    		        }
    		    }
    		    bool is_pressed = glob_k_stat[ac_code];
    		    glob_k_stat[ac_code] = false;
    		    return (double)(is_pressed ? 1 : 0);
    		#else
    			#error "expected unknown OS"
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
	        if(ev_args.size() == 1) {
	            if(!holds_alternative<string>(ev_args[0])) { 
                    execute_error("current type != STRING",node);
	                return ErrorValue{};
	            }
	            string cmd = get<string>(ev_args[0]);
	            #if defined(__linux__) || defined(__APPLE__)
	            FILE* pipe = popen(cmd.c_str(),"r");
	            #elif defined(_WIN32) || defined(_WIN64)
	            FILE* pipe = _popen(cmd.c_str(),"r");
	            #else
	            #error "unknown platform"
	            #endif
	            if(!pipe) {
	                execute_error("popen failed",node);
	                return ErrorValue{};
	            }	            
	            char buffer[256];
	            string result = "";
	            while(fgets(buffer,sizeof(buffer),pipe) != nullptr) {
	                result += buffer;
	            }
                #if defined(__linux__) || defined(__APPLE__)
	            pclose(pipe);
	            #elif defined(_WIN32) || defined(_WIN64)
	            _pclose(pipe);
	            #else
	            #error "unknown platform"
	            #endif
	            return result;
	        }else {
 	            execute_error("expected exactly one argument",node);
      	        return ErrorValue{};
 	        }
	    }
	    if(name == "__builtin_chdir") {
	        if(ev_args.size() == 1) {
	            if(!holds_alternative<string>(ev_args[0])) { 
                    execute_error("current type != STRING",node);
	                return ErrorValue{};
	            }
	            string path = get<string>(ev_args[0]);
	            int cd = -1;
	            #if defined(__linux__) || defined(__APPLE__)
	            cd = chdir(path.c_str());
	            #elif defined(_WIN32) || defined(_WIN64)
                cd = SetCurrentDirectoryA(path.c_str()) ? 0 : -1;
	            #else
	            #error "unknown platform to chdir"
	            #endif
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
	        #if defined(__APPLE__) && defined(__MACH__)
	            return string("macos");
	        #elif defined(__ANDROID__)
	            return string("android");
	        #elif defined(__linux__)
	            return string("linux");
	        #elif defined(_WIN32) || defined(_WIN64)
                return string("windows");
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
	        execute_error("not found function",node);
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
        if(filename.empty()) {
          cout<<"\033[1;31mE: file is empty()\033[0m"<<endl;
          return ErrorValue{};
        }
        string home = "";
        #if defined(_WIN32) || defined(_WIN64)
        if(const char* win_home = getenv("USERPROFILE")) home = win_home;
        #else
        if(const char* unix_home = getenv("HOME")) home = unix_home;
        #endif
        filesystem::path sys_dir = filesystem::path(home) / "LemonenLang" / "libs" / filename;
        filesystem::path local_dir = filename;
        filesystem::path local_libs = filesystem::path("libs") / filename;
        filesystem::path target_path;
        if(filesystem::exists(sys_dir)) { target_path = sys_dir; }
        else if(filesystem::exists(local_dir)) { target_path = local_dir; }
        else if(filesystem::exists(local_libs)) { target_path = local_libs; }
        else { execute_error("file not found " + filename,node); return ErrorValue{}; }
        ifstream file(target_path);
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
	        execute_error("you can't measure the length of numbers ",node);
	        return ErrorValue{"E: you can't measure the length of numbers"};
	    }
	    if(holds_alternative<bool>(val)) {
	        execute_error("you can't measure the length of bools",node);
	        return ErrorValue{"E: you can't measure the length of bools"};
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
	    if(holds_alternative<shared_ptr<DictValue>>(right)) {
	    	return string("DICT");
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
                execute_error("array index must be integer",node);
                return ErrorValue{"E: array index must be integer"};
            }
            int right = (int)index_double;
            if(right < 0 || (size_t)right >= left->elements.size()) {
                execute_error("ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()",node);
                return ErrorValue {"E: ST_INDEX < 0 || ST_INDEX > ST_ARRAY.size()"};
            }
            return left->elements[right];
        }
        if(holds_alternative<string>(left_val)) {
            if(!holds_alternative<double>(right_val)) {
                execute_error("dont have ST_INDEX in ST_ARRAY",node);
                return ErrorValue {"E: dont have ST_INDEX in ST_ARRAY"};
            }
            string left = get<string>(left_val);
            double index_double = get<double>(right_val);
            if(index_double != (int)index_double) {
                execute_error("string index must be integer",node);
                return ErrorValue{"E: string index must be integer"};
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
                return finder->second;
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
		string op = node->VAL;
		if(holds_alternative<ErrorValue>(left_val)) { return left_val; }
		if(holds_alternative<ErrorValue>(right_val)) { return right_val; }
		if(holds_alternative<double>(left_val) && holds_alternative<double>(right_val)) {
	        double left = get<double>(left_val);
	        double right = get<double>(right_val);
			if(op == "+") { double mo = left + right; return mo; }
			else if(op == "-") { double mo = left - right; return mo; }
			else if(op == "/") {
				if(right != 0) {
					double mo = left / right;  return mo; 
				}
				else {
				    execute_error("cannot be divided by zero",node);
					return ErrorValue{"E: cannot be divided by zero"};
				}
			}				
			else if(op == "*") { double mo = left * right;  return mo;  }
			else if(op == "%") { 
			    if(right != 0) {
			        double mo = fmod(left,right); return mo; 
			    }else {
			        execute_error("cannot be divided by zero",node);
	  				return ErrorValue{"E: cannot be divided by zero"};    
			    }
			}
		}
		if(holds_alternative<shared_ptr<DictValue>>(left_val) || holds_alternative<shared_ptr<DictValue>>(right_val)) {
			if(op == "+") {
				auto left = get<shared_ptr<DictValue>>(left_val);
				auto right = get<shared_ptr<DictValue>>(right_val);
				auto res = make_shared<DictValue>();
				res->dict_val = left->dict_val;
				for(const auto& pair : right->dict_val) {
					res->dict_val[pair.first] = pair.second;
				}
				return res;
			}
			else {
				execute_error("unknown operator for dictionary",node);
				return ErrorValue{};
			}
		}
		if(holds_alternative<shared_ptr<ArrayValue>>(left_val) || holds_alternative<shared_ptr<ArrayValue>>(right_val)) {
			if(op == "+") {
				auto left = get<shared_ptr<ArrayValue>>(left_val);
				auto right = get<shared_ptr<ArrayValue>>(right_val);
				auto res = make_shared<ArrayValue>();
				res->elements.reserve(left->elements.size() + right->elements.size());
				res->elements.insert(res->elements.end(),left->elements.begin(),left->elements.end());
				res->elements.insert(res->elements.end(),right->elements.begin(),right->elements.end());
				return res;
			}
			else if(op == "*") {
				if(holds_alternative<double>(left_val) && holds_alternative<shared_ptr<ArrayValue>>(right_val)) {
					auto right = get<shared_ptr<ArrayValue>>(right_val);
					double left = get<double>(left_val);
					if(isnan(left)) {
						execute_error("left element is NaN",node);
						return ErrorValue{};
					}
					if(isinf(left)) {
						execute_error("left element is inf",node);
						return ErrorValue{};
					}
					if(left <= 0) {
						return make_shared<ArrayValue>();
					}
					if(left == 1) {
						return right;
					}
					size_t count = static_cast<size_t>(left);
					if(count * right->elements.size() > 1000000) {
						execute_error("right->elements.size() > 1000000",node);
						return ErrorValue{};
					}
					auto res = make_shared<ArrayValue>();
					unsigned int i = 0;
					res->elements.reserve(right->elements.size() * count);
					while(i < count) {
						res->elements.insert(res->elements.end(),right->elements.begin(),right->elements.end());
						i++;
					}
					return res;
				}
				if(holds_alternative<shared_ptr<ArrayValue>>(left_val) && holds_alternative<double>(right_val)) {
					double right = get<double>(right_val);
					auto left = get<shared_ptr<ArrayValue>>(left_val);
					if(isnan(right)) {
						execute_error("left element is NaN",node);
						return ErrorValue{};
					}
					if(isinf(right)) {
						execute_error("left element is inf",node);
						return ErrorValue{};
					}
					if(right <= 0) {
						return make_shared<ArrayValue>();
					}
					if(right == 1) {
						return left;
					}
					size_t count = static_cast<size_t>(right);
					if(count * left->elements.size() > 1000000) {
						execute_error("left->elements.size() > 1000000",node);
						return ErrorValue{};
					}
					auto res = make_shared<ArrayValue>();
					unsigned int i = 0;
					res->elements.reserve(left->elements.size() * count);
					while(i < count) {
						res->elements.insert(res->elements.end(),left->elements.begin(),left->elements.end());
						i++;
					}
					return res;
				}
			}else {
				execute_error("unknown operator for array",node);
				return ErrorValue{};
			}
		}
		if(holds_alternative<string>(left_val) || holds_alternative<string>(right_val)) {
			if(op == "+") {
				string left = get<string>(left_val);
				string right = get<string>(right_val);
				string res = left + right; return res;
			}
			else if(op == "*") {
				if(holds_alternative<double>(left_val) && holds_alternative<string>(right_val)) {
					string right = get<string>(right_val);
					double left = get<double>(left_val);
					if(isnan(left)) {
						execute_error("left element is NaN",node);
						return ErrorValue{};
					}
					if(isinf(left)) {
						execute_error("left element is inf",node);
						return ErrorValue{};
					}
					if(left > 1000000) {
						execute_error("expected limit violation,max: 1.000.000",node);
						return ErrorValue{};
					}
					if(left <= 0) {
						return "";
					}
					if(left == 1) {
						return right;
					}
					size_t count = static_cast<size_t>(left); 
					string res = "";
					unsigned int i = 0;
					while(i < count) {
						res = res + right;
						i++; 
					}
					return res;
				}
				if(holds_alternative<string>(left_val) && holds_alternative<double>(right_val)) {
					string left = get<string>(left_val);
					double right = get<double>(right_val);
					if(isnan(right)) {
						execute_error("right element is NaN",node);
						return ErrorValue{};
					}
					if(isinf(right)) {
						execute_error("right element is inf",node);
						return ErrorValue{};
					}
					if(right > 1000000) {
						execute_error("expected limit violation,max: 1.000.000",node);
						return ErrorValue{};
					}
					if(right <= 0) {
						return "";
					}
					if(right == 1) {
						return left;
					}
					size_t count = static_cast<size_t>(right); 
					string res = "";
					unsigned int i = 0;
					while(i < count) {
						res = res + left;
						i++; 
					}
					return res;
				}
			}else {
				execute_error("unknown operation for string",node);
				return ErrorValue{};
			}
		}
	}
	else if(node->KEY == ST_ASSIGNMENT) {
        if(node->right_children.empty()) {
            execute_error("right_elements is empty()",node);
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
            for(const Value& ev : right_values) {
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
                        Value item = getter->elements[i];
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
            execute_error("node->children is empty()",node);
            return ErrorValue {"E: node->children is empty()"};
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
        string input;
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
        #if defined(__linux__) || defined(__APPLE__)
        fflush(stdout);
        char* input_ptr = readline(prompt.c_str());
        if(input_ptr == nullptr) {
            execute_error("input interrupted",node);
            return ErrorValue{"E: input interrupted"};
        }
        input = input_ptr;
        free(input_ptr);
        #elif defined(_WIN32) || defined(_WIN64)
        if(!prompt.empty()) {
            cout<<prompt<<flush;
        }
        if(!getline(cin,input)) {
            execute_error("input interrupted",node); 
            return ErrorValue{};
        };
        #else
        #error "unknown platform"
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
