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

		std::vector<Instruction> instructions;
		size_t ip;

		bool run_instr(const Instruction& instr);

		/// Given a vector of value types, validates the stack FROM the back and returns void if the stack matches expected values. Else returns an exception. Also returns an exception when the size of the expected_values is greater than the values currently in the stack
		// Example: if the stack is currently holding values of type { i32, i64, f32 }, and the expected the values are {i64, f32}, the function will first check if the size of the stack is less than the expected values and throw an exception if it is.
		// Then, it'll loop over from the back of both the stacks until the expected_values vector is completely iterated. So, it'll first check if last element from the expected_values vector (which is a f32) matches the last element of the stack (which is also a f32). Then it moves on to the second last element and so on
		// Example: when stack = {f32,f64} and expected_values = {f32}, it'll throws an exception as we expected the top to be f32 but we got f64.
		// NOTE: when stack = {f32,f64} and expected_values = {},       it'll NOT throw an exception. See if expect_stack_exact() fits your use case
		void expect_stack(std::vector<ValueType> expected_values);

		void expect_stack_exact(std::vector<ValueType> expected_values);
};
#endif
