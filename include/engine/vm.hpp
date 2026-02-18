#ifndef VM_HPP
#define VM_HPP

#include<optional>
#include<stack>
#include"value.hpp"

class VM {
	public:
		VM();
		void push(Value);
		std::optional<Value> pop();

	private:
		std::vector<Value> stack
};
#endif
