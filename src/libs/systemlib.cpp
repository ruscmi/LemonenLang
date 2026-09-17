#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#elif defined(__linux__)
#include <linux/input.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif
#include <cstdlib>
#include <filesystem>
#include "../../include/libs.hpp"
#define execute_error(msg,node) interpreter::execute_error(msg,node)
using namespace std;
optional<Value>builtin_system(const string& name,const vector<Value>& ev_args,Node* node,bool is_sys) {
	if(name == "__builtin_keypressed") {
	    if(ev_args.size() != 1) {
	    	execute_error("expected arguments < 1",node);
	    	return ErrorValue{};
	    }
	    if(!holds_alternative<string>(ev_args[0])) {
	    	execute_error("zero arg is not string",node);
	    	return ErrorValue{};
	    }
  	    	string key_name = get<string>(ev_args[0]);
    	#if defined(_WIN32) || defined(_WIN64)
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
   		auto it = KEY_MAP.find(key_name);
   		if(it == KEY_MAP.end()) {
   			return (double)0;
   		}
   		int ac_code = it->second;
   			SHORT state = GetAsyncKeyState(ac_code);
   			bool is_pressed = (state & 0x8000) != 0;
   			return (double)(is_pressed ? 1 : 0);
   		#elif defined(__linux__)
				struct termios oldt,newt;
				tcgetattr(STDIN_FILENO,&oldt);
				newt = oldt;
				newt.c_lflag &= ~ICANON;
				newt.c_lflag &= ~ECHO;
				newt.c_cc[VMIN] = 0;
				newt.c_cc[VTIME] = 0;
				tcsetattr(STDIN_FILENO,TCSANOW,&newt);
				char buf[8] = {0}; 
				int bytes_read = read(STDIN_FILENO,buf,sizeof(buf));
				tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
				if(bytes_read <= 0) {
					return (double)0;
				}
				string input_seq(buf,bytes_read);
				static const unordered_map<string, string> LINUX_KEY_MAP = {
			        {"w", "w"}, {"e", "e"}, {"q", "q"}, {"r", "r"}, {"t", "t"},
			        {"y", "y"}, {"u", "u"}, {"i", "i"}, {"o", "o"}, {"p", "p"},
			        {"a", "a"}, {"s", "s"}, {"d", "d"}, {"f", "f"}, {"g", "g"},
			        {"h", "h"}, {"j", "j"}, {"k", "k"}, {"l", "l"}, {"z", "z"},
			        {"x", "x"}, {"c", "c"}, {"v", "v"}, {"b", "b"}, {"n", "n"},
			        {"m", "m"}, {"space", " "}, {"enter", "\n"}, {"tab", "\t"},
			        {"backspace", "\b"}, {"esc", "\033"},
			        {"up", "\033[A"}, {"down", "\033[B"}, 
			        {"right", "\033[C"}, {"left", "\033[D"},
			        {"home", "\033[H"}, {"end", "\033[F"},
   			        {"insert", "\033[2~"}, {"delete", "\033[3~"},
   			        {"pageup", "\033[5~"}, {"pagedown", "\033[6~"},
   			        {"f1", "\033OP"}, {"f2", "\033OQ"}, {"f3", "\033OR"}, {"f4", "\033OS"},
   			        {"f5", "\033[15~"}, {"f6", "\033[17~"}, {"f7", "\033[18~"}, {"f8", "\033[19~"},
   			        {"f9", "\033[20~"}, {"f10", "\033[21~"}, {"f11", "\033[23~"}, {"f12", "\033[24~"},
   			        {"+","+"},{"-","-"},{"/","/"},{".","."},{",",","}
				};
			    auto it = LINUX_KEY_MAP.find(key_name);
		        if (it != LINUX_KEY_MAP.end() && input_seq == it->second) {
		            return (double)1;
		        }
				return (double)0;
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
	return nullopt;
}
struct systemreg {
	systemreg() {
		get_res_libs().push_back(builtin_system);
	}
};
static systemreg systemlibreg;
