#include "../../include/libs.hpp"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
#define execute_error(msg,node) interpreter::execute_error(msg,node)
optional<Value>builtin_filereg(const string& name,const vector<Value>& ev_args,Node* node,bool is_sys) {
	if(name == "__builtin_fileread") {
		if(ev_args.size() == 1) {
			if(!holds_alternative<string>(ev_args[0])) {
				execute_error("argument file_read is not string",node);
				return ErrorValue{};
			}
			string filename = get<string>(ev_args[0]);
			ifstream file(filename);
			if(!file.is_open()) {
				execute_error("file return unknown value or does not exist",node);
				return ErrorValue{};
			}
			stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			string content = buffer.str();
			return content;
		}else {
			execute_error("file_read func() need a one argument(file or filepath)",node);
			return ErrorValue{};
		}
	}
	if(name == "__builtin_filewrite") {
		if(ev_args.size() == 2) {
			if(!holds_alternative<string>(ev_args[0]) || !holds_alternative<string>(ev_args[1])) {
				execute_error("need args: file_write(\"filename\",\"text to file writing\")",node);
				return ErrorValue{};
			}
			string filename = get<string>(ev_args[0]);
			string code = get<string>(ev_args[1]);
			ofstream file(filename);
			if(!file.is_open()) {
				execute_error("file return unknown value or does not exist",node);
				return ErrorValue{};
			}
			file << code;
			file.close();
			return true;
		}else {
			execute_error("file_write func() need a two arguments",node);
			return ErrorValue{};
		}
	}
	return nullopt;
}
struct filereg {
	filereg() {
		get_res_libs().push_back(builtin_filereg);
	}
};
static filereg filelibreg;
