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
		/// Stores the indices where to jump into the `instructions` vector when a br is reached, depending on the type of the block (whether loop or break etc.)
		// Example: if the instruction br 2, it'll pop 2 elements from this vector and jump the the latest popped element.
		std::vector<size_t> br_labels; 

		bool run_instr(const Instruction& instr);

		std::vector<Instruction> instructions;
		size_t ip;

};
#endif
