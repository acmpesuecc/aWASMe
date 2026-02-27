#ifndef VM_HPP
#define VM_HPP

#include<optional>
#include<stack>
#include<vector>
#include<variant>

#include"value.hpp"
#include "instruction.hpp"
#include"funcs.hpp"

class ControlFrame {	
	private:
		std::variant<Block,ActivationRecord> inner;
	public:
		ControlFrame(Block b,size_t init_sp): inner(b),initial_sp(init_sp) {};
		ControlFrame(BlockInfo bi,BrAction bt,size_t sp): inner(Block{.br_action = bt, .info = bi}), initial_sp(sp)  {};
		ControlFrame(ActivationRecord a,size_t sp): inner(a),initial_sp(sp) {};

		size_t initial_sp;

		/// Returns an optional index to which the ip will be set, if a br, or a return or end in the case of functions is run.
		std::optional<size_t> get_branch_target(); 

		bool is_block();
		bool is_activation_record();

		std::optional<Block> get_block();
		std::optional<ActivationRecord> get_activation_record();
		std::optional<ValueType> get_return_type();
};

class VM {
	public:

		VM();
		
		/// Value stack operations
		void push(Value);
		std::optional<Value> pop();

		void load(const std::vector<Instruction>& instrs);
		/// Running the instructions which were loaded into the `instructions` member, starting from the instruction pointer.
		/// May return a VMError exception which any of the assumptions of the instructions fails during runtime.
		/// Also updates the ip after running each instruction successfully. 
		void run();

		// Registers a function into the VM and returns the index which will be used to refer to it.
		size_t register_function(FunctionInfo f);


	private:
		std::vector<Value> stack;

		std::vector<Instruction> instructions;
		size_t ip;

		std::vector<ControlFrame> control_frames;

		// Currently, only stores WASM functions and not imported ones
		// NOTE: In the future when imported functions are supported, external functions must occupy lower indicies than WASM defined functions. In other words, register all imported functions first, then WASM defined one
		std::vector<FunctionInfo> functions; 

		bool run_instr(const Instruction& instr);

		/// Given a vector of value types, validates the stack from the BACK returns an exception if the values mismatch. Also returns an exception when the size of the expected_values is greater than the values currently in the stack
		// Example: if the stack is currently holding values of type { i32, i64, f32 }, and the expected the values are {i64, f32}, the function will first check if the size of the stack is less than the expected values and throw an exception if it is.
		// Then, it'll loop over from the back of both the stacks until the expected_values vector is completely iterated. So, it'll first check if last element from the expected_values vector (which is a f32) matches the last element of the stack (which is also a f32). Then it moves on to the second last element and so on
		// Example: when stack = {f32,f64} and expected_values = {f32}, it'll throws an exception as we expected the top to be f32 but we got f64.
		// NOTE: when stack = {f32,f64} and expected_values = {},       it'll NOT throw an exception. See if expect_stack_exact() fits your use case
		void expect_stack(std::vector<ValueType> expected_values);

		/// Given a vector of value types, validates the stack from the BACK and returns. Unlike expect_stack, this function needs the stack to be exactly equal to expected_values.
		void expect_stack_exact(std::vector<ValueType> expected_values);
};
#endif
