#include"vm.hpp"

VM::VM() {
	this->stack = {};
}

void VM::push(Value v) {
	this->stack.push_back(v);
}

std::optional<Value> VM::pop() {
	if(this->stack.empty()) return {};

	Value top = this->stack.back();
	this->stack.pop_back();
	return top;
}
