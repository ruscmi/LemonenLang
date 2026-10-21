#include <iostream>
#include <optional>
#include <cmath>
#include "../../include/libs.hpp"
#define execute_error(msg,node) interpreter::execute_error(msg,node)
using namespace std;
optional<Value>builtin_math(const string& name,const vector<Value>& ev_args,Node* node,bool is_sys) {
	if(name == "__builtin_nan") {
		return double(nan(""));
	}
	return nullopt;
}
struct mathlibreg {
	mathlibreg() {
		get_res_libs().push_back(builtin_math);
	}
};
static mathlibreg mathlib;
