#ifndef VM_HPP
#define VM_HPP

#include<optional>
#include<stack>
#include"value.hpp"
#include<vector>

class VM {
	public:
		VM();
		void push(Value);
		std::optional<Value> pop();

		// TODO: Create a run_instr function which runs an instruction and updates the ip

	private:
		std::vector<Value> stack;
		//TODO: add a vector of instructions, along with instruction pointer 'ip'
		//TODO: Week 3: add activation record stack 

};
#endif
