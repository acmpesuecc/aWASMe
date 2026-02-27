#ifndef FUNCS_HPP
#define FUNCS_HPP

#include<vector>

#include"instruction.hpp"
#include"value.hpp"

struct FunctionInfo {
	BlockInfo block_info;
	std::vector<ValueType> args;
};

class ActivationRecord {
	private:
		FunctionInfo& info;
		std::vector<Value> locals; // includes arguments, which always occupy lower indicies than locals defined inside function body
	public:
		ActivationRecord(FunctionInfo& _info,std::vector<Value> locals_):info(_info), locals(locals_){};

		std::optional<size_t> return_to; // the raw index where ip should be set to when this function encounters a return instruction
		std::optional<ValueType> get_return_type();
};
#endif
