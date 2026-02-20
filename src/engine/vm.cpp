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

void VM::load(const std::vector<Instruction>& instrs) {
    instructions = instrs;
    ip = 0;
}

bool VM::run_instr(const Instruction& instr) {
    switch (instr.kind) {

        case InstrKind::I32_CONST: {
            int32_t value = std::get<int32_t>(*instr.operand);
            push(value);
            return true;
        }

        case InstrKind::I32_ADD: {
            auto b = std::get<int32_t>(*pop());
            auto a = std::get<int32_t>(*pop());
            push(a + b);
            return true;
        }

        case InstrKind::I32_SUB: {
            auto b = std::get<int32_t>(*pop());
            auto a = std::get<int32_t>(*pop());
            push(a - b);
            return true;
        }

        case InstrKind::I32_MUL: {
            auto b = std::get<int32_t>(*pop());
            auto a = std::get<int32_t>(*pop());
            push(a * b);
            return true;
        }

        case InstrKind::END: {
            return false;
        }
    }
    return true;
	
}

void VM::run() {
    while (ip < instructions.size()) {
        if (!run_instr(instructions[ip]))
            break;
        ip++;
    }
}