#include"instruction.hpp"

std::string Instruction::to_string() {
	    switch(this->kind) {
		    case InstrKind::I32_CONST: return "i32.const " + ::to_string(this->args.value().value);
		    case InstrKind::I32_ADD: return "i32.add";
		    case InstrKind::I32_SUB: return "i32.sub";
		    case InstrKind::I32_MUL: return "i32.mul" ;
		    case InstrKind::BLOCK: return "block";
		    case InstrKind::LOOP: return "loop";
		    case InstrKind::BR: return "br" + ::to_string(this->args.value().value);
		    case InstrKind::IF: return "if";
		    case InstrKind::END: return "end";
	    }
	__builtin_unreachable();
}
