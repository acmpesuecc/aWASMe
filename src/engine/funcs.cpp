#include "engine/funcs.hpp"

const char* EXPORTED_FNS = "exported_fns";

std::optional<ValueType> ActivationRecord::get_return_type() {
	return this->info.return_type;	
}

void ActivationRecord::push_local(Value v) {
	this->locals.push_back(v);
}
std::optional<Value> ActivationRecord::get_local(size_t index) {
	if(this->locals.size() <= index) return {};

	return this->locals.at(index);
}

void ActivationRecord::set_local_raw(size_t index,Value v) {
	if(this->locals.size() <= index) return; 
	this->locals[index] = v;
}

BlockInfo ActivationRecord::get_func_block_info() {
	return this->info;
}
