/*
	lmnlang - GPL v2.0 - see LICENSE or main.cpp file for details
*/
#if defined(__ANDROID)
#include <ctime>
#endif
#include <chrono>
#include <thread>
#include "../../include/libs.hpp"
#define execute_error(msg,node) interpreter::execute_error(msg,node)
using namespace std;
optional<Value>builtin_time(const string& name,const vector<Value>& ev_args,Node* node,bool is_sys) {
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
    return nullopt;
}
struct timelibreg {
	timelibreg() {
		get_res_libs().push_back(builtin_time);
	}
};
static timelibreg timelib;
