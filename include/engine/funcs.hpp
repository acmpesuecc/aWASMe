#ifndef FUNCS_HPP
#define FUNCS_HPP
#include<vector>
#include<variant>

#include "module/instruction.hpp"

extern const char* EXPORTED_FNS;

struct InternalFunction {
	std::vector<ValueType> locals; 
	BlockInfo block_info;
};

struct ImportedFunction {
	size_t index;
	std::optional<ValueType> return_type;
};

using FunctionKind = std::variant<InternalFunction,ImportedFunction>;

struct Function{
	std::vector<ValueType> args;
	FunctionKind kind;
};

class ActivationRecord {
	private:
		BlockInfo& info;
		std::vector<Value> locals; // includes arguments, which always occupy lower indicies than locals defined inside function body
		std::optional<ValueType> return_type;
	public:

		ActivationRecord(BlockInfo& _info,std::vector<Value> locals_):info(_info), locals(locals_){};

		std::optional<size_t> return_to; // the raw index where ip should be set to when this function encounters a return instruction
		std::optional<ValueType> get_return_type();

		void push_local(Value v);
		std::optional<Value> get_local(size_t index);
		void set_local_raw(size_t index,Value v);

		BlockInfo get_func_block_info();
};

#endif
