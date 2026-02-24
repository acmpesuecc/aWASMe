#include"instruction.hpp"
std::string Instruction::to_string() {
	    switch(this->kind) {
		    case InstrKind::I32_CONST: return "i32.const " + ::to_string(this->args.value().value);
		    case InstrKind::I32_ADD: return "i32.add";
		    case InstrKind::I32_SUB: return "i32.sub";
		    case InstrKind::I32_MUL: return "i32.mul" ;
		    case InstrKind::BLOCK: return "block";
		    case InstrKind::LOOP: return "loop";
		    case InstrKind::BR: return "br " + std::to_string(this->args.value().index);
		    case InstrKind::IF: return "if";
		    case InstrKind::END: return "end";
		    case InstrKind::CALL: return "call " + std::to_string(this->args.value().index);
		    case InstrKind::RETURN: return "return";
		    case InstrKind::NOP: return "nop";
		    case InstrKind::UNREACHABLE: return "unreachable";
	    }
	__builtin_unreachable();
}
