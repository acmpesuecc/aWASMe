#ifndef VM_HPP
#define VM_HPP

#include<optional>
#include<stack>
#include<vector>
#include"value.hpp"
#include "instruction.hpp"

class VM {
	public:
		VM();
		void push(Value);
		std::optional<Value> pop();

		void load(const std::vector<Instruction>& instrs);
    	void run();

	private:
		std::vector<Value> stack;

		bool run_instr(const Instruction& instr);

		std::vector<Instruction> instructions;
		size_t ip;

};
#endif
