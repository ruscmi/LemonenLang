#include "../../include/libs.hpp"
#include <vector>
#include <functional>

using LibExecFunc = std::optional<Value> (*)(const std::string&,const std::vector<Value>&,Node*,bool);
std::vector<LibExecFunc>& get_res_libs() {
	static std::vector<LibExecFunc>libs;
	return libs;
}
std::optional<Value>builtin_exec(const std::string& name,const std::vector<Value>& ev_args,Node* node,bool is_sys) {
	for(auto lib_f : get_res_libs()) {
		if(auto res = lib_f(name,ev_args,node,is_sys)) {
			return res;
		}
	}
	return std::nullopt;
}
