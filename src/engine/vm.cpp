#include<vector>
#include<optional>

#include"value.hpp"

class VM {
	private:
		std::vector<Value> stack;
		//TODO: add a vector of instructions, along with instruction pointer 'ip'
		//TODO: Week 3: add activation record stack 
	
	public:
		VM() {
			this->stack = {};
		}

		void push(Value v) {
			this->stack.push_back(v);
		}

		std::optional<Value> pop(Value v) {
			if(this->stack.empty()) return {};

			Value top = this->stack.back();
			this->stack.pop_back();
			return top;
		}


		// TODO: Create a run_instr function which runs an instruction and updates the ip
};
