#include "../include/libs.hpp"
// for android recommended '//' this define
#define ENABLE_GUI
#ifdef ENABLE_GUI
#define RAYLIB_IMPLEMENTATION
#include "../include/raylib.h"
#include "../include/raygui.h"
#endif
#include "../include/interpreter.hpp"
using namespace std;
#define execute_error interpreter::execute_error
optional<Value>builtin_exec(const string& name,const vector<Value>& ev_args,Node* node,bool is_sys) {
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
	    	#if defined(__ANDROID__) 
	    	setenv("TZ",get_st.c_str(),1);
	    	tzset();
	    	time_t t = time(nullptr);
	    	cout<< put_time(localtime(&t),"%Y-%m-%d %H:%M:%S") << "\n";
	    	#else
	    	auto local_zone = chrono::locate_zone(get_st);
	    	auto time_zone = local_zone->to_local(now);
	    	cout<<std::format("{:%Y-%m-%d %H:%M:%S}\n",time_zone);
	    	#endif
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
				char buf[3] = {0}; 
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
			        {"right", "\033[C"}, {"left", "\033[D"}
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
#ifdef ENABLE_GUI
	/* GUI __builtin for devs */
    if(name == "__builtin_winshoclose") {
    	return (double)!WindowShouldClose();
    }
    if(name == "__builtin_begdrawing") {
    	BeginDrawing();
    	return AcceptValue{};
    }
    if(name == "__builtin_drawtext") {
    	if(ev_args.size() >= 8) {
    		if(!holds_alternative<string>(ev_args[0]) || !holds_alternative<double>(ev_args[1]) ||
    		!holds_alternative<double>(ev_args[2]) || !holds_alternative<double>(ev_args[3])  ||
    		!holds_alternative<double>(ev_args[4]) || !holds_alternative<double>(ev_args[5]) || 
    		!holds_alternative<double>(ev_args[6]) || !holds_alternative<double>(ev_args[7])){
    			execute_error("unvalidate data-type in parametr lgui_textdrawing()",node);
    			return ErrorValue{};
    		}
    		string text = get<string>(ev_args[0]);
  				int x = static_cast<int>(get<double>(ev_args[1]));
  				int y = static_cast<int>(get<double>(ev_args[2]));
  				int fsize = static_cast<int>(get<double>(ev_args[3]));
  				unsigned char r = static_cast<unsigned char>(get<double>(ev_args[4]));
  				unsigned char g = static_cast<unsigned char>(get<double>(ev_args[5]));
  				unsigned char b = static_cast<unsigned char>(get<double>(ev_args[6]));
  				unsigned char a = static_cast<unsigned char>(get<double>(ev_args[7]));
  				DrawText(text.c_str(),x,y,fsize,Color{r,g,b,a});
    	}
    	return AcceptValue{};
    }
    if(name == "__builtin_colordraw") {
    	if(ev_args.size() >= 3) {
    		if(!holds_alternative<double>(ev_args[0]) || !holds_alternative<double>(ev_args[1])
    		|| !holds_alternative<double>(ev_args[2])) {
				execute_error("unvalidate data-type in lgui_colordrawing()",node);
    			return ErrorValue{};
    		}
    		double c_1 = get<double>(ev_args[0]);
    		double c_2 = get<double>(ev_args[1]);
    		double c_3 = get<double>(ev_args[2]);
    		unsigned char con_1 = static_cast<unsigned char>(c_1);
    		unsigned char con_2 = static_cast<unsigned char>(c_2);
			unsigned char con_3 = static_cast<unsigned char>(c_3);
	    	ClearBackground(Color { con_1,con_2,con_3,255 });
    	}
    	return AcceptValue{};
    }
    if(name == "__builtin_enddraw") {
    	EndDrawing();
    	return AcceptValue{};
    }
    if(name == "__builtin_closewin") {
    	CloseWindow();
    	return AcceptValue{};
    }
    if(name == "__builtin_setfps") {
    	if(ev_args.size() >= 1 ) {
    		if(holds_alternative<double>(ev_args[0])) {
    			double fps = get<double>(ev_args[0]);
    			SetTargetFPS(fps);
    		}
    	}
    	return AcceptValue{};
    }
    if(name == "__builtin_initw") {
    	if(ev_args.size() >= 3) {
    		if(!holds_alternative<double>(ev_args[0]) && !holds_alternative<double>(ev_args[1])
    		&& !holds_alternative<string>(ev_args[2])) {
    			execute_error("execute unknown data-type parametrs",node);
    		}else {
    			double x_par = get<double>(ev_args[0]);
    			double y_par = get<double>(ev_args[1]);
    			string name = get<string>(ev_args[2]);
    			SetTraceLogLevel(LOG_NONE);
    			InitWindow(x_par,y_par,name.c_str());
    			cout<<"[LMNGUI]: Load complete. lmngui by ruscmi\n";
    			cout<<"[LMNGUI]: Im love lemons. github.com/ruscmi/LemonenLang\n";
    		}
    	}
    	return AcceptValue{};
    }
#endif	
	return nullopt;
}
