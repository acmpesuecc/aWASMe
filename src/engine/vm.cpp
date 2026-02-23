#include "engine/vm.hpp"

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
	    InstrArgs args = instr.args.value();
            int32_t value = std::get<int32_t>(args.value);
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
	    size_t loc = this->br_labels.back(); 
	    this->br_labels.pop_back();
 	    this->ip = loc;
	    return true;
        }

	case InstrKind::BLOCK: {
	    InstrArgs args = instr.args.value();
	    auto bk = args.block_kind;
	    this->br_labels.push_back(bk.block_end);
	    return true;
	}

	case InstrKind::LOOP: {
	    InstrArgs args = instr.args.value();
	    auto bk = args.block_kind;
	    this->br_labels.push_back(bk.block_start);
	    return true;
	}

	case InstrKind::BR: {
	    InstrArgs args = instr.args.value();
	    int32_t bk = std::get<int32_t>(args.value);
	    size_t loc;	
	    for(int i = 0;i<=bk;i++) {
	         loc = this->br_labels.back(); 
		 this->br_labels.pop_back();
	    }
	    this->ip = loc;
	    return true;
	}

	case InstrKind::IF: {
	    InstrArgs args = instr.args.value();
	    auto if_blk_info = args.if_.if_block_info; 
	    auto else_blk_info_opt = args.if_.else_block_info; 

	    Value val = this->pop().value(); // TODO: throw errors pop returns null 
            int32_t v = std::get<int32_t>(val); // TODO: handle error  
	
	    decltype(if_blk_info) blk_info; 

	    if(v>0) {
	           blk_info = if_blk_info;
	    }else if(else_blk_info_opt.has_value()) {
	           blk_info = else_blk_info_opt.value();
	    }else {
	           // skip the block entirely
		   this->ip = if_blk_info.block_end; 
		   return true;
	    }

	    this->br_labels.push_back(blk_info.block_end);
	    this->ip = blk_info.block_start; 
	    return true;
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
