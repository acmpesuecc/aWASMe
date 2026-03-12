#include"instruction.hpp"

std::string to_string(Instruction i) {
	const auto visitor = overloads {
		[](Nop&) { return std::string("nop"); },
		[](Unreachable&) { return std::string("unreachable"); },
		[](LoadConst& lc) {
			Value v = lc.value;
			std::string out = "";
			switch(to_value_type(v)) {
				case ValueType::i32: 
					out += "i32.const";
					break;
				case ValueType::i64: 
					out += "i64.const";
					break;
				case ValueType::f32: 
					out += "f32.const";
					break;
				case ValueType::f64: 
					out += "f64.const";
					break;
			}
			out += " " + to_string(v);
			return out;

		},
		[](IntArithmetic& a) { 
			std::string out = a.num_type == IntType::i32 ? "i32.": "i64.";
			switch(a.op_kind) {
				case IntArithmetic::Kind::Add:
					out += "add";
					break;
				case IntArithmetic::Kind::Sub:
					out += "sub";
					break;
				case IntArithmetic::Kind::Mul:
					out += "mul";
					break;
				case IntArithmetic::Kind::DivS:
					out += "div_s";
					break;
				case IntArithmetic::Kind::RemS:
					out += "rem_s";
					break;
				case IntArithmetic::Kind::DivU:
					out += "div_u";
					break;
				case IntArithmetic::Kind::RemU:
					out += "rem_u";
					break;
			}

			return out;
		},
		[](FloatArithmetic& a) { 
			std::string out = a.num_type == FloatType::f32 ? "f32.": "f64.";
			switch(a.op_kind) {
				case FloatArithmetic::Kind::Add:
					out += "add";
					break;
				case FloatArithmetic::Kind::Sub:
					out += "sub";
					break;
				case FloatArithmetic::Kind::Mul:
					out += "mul";
					break;
			}
			return out;
		},
		[](FloatCmp& c) {
			std::string out = c.num_type == FloatType::f32 ? "f32.": "f64.";
			switch(c.op_kind) {
				case FloatCmp::Kind::Eq:
					out += "eq";
					break;
				case FloatCmp::Kind::Ne:
					out += "ne";
					break;
				case FloatCmp::Kind::Lt:
					out += "lt";
					break;
				case FloatCmp::Kind::Gt:
					out += "gt";
					break;
				case FloatCmp::Kind::Le:
					out += "le";
					break;
				case FloatCmp::Kind::Ge:
					out += "ge";
					break;
			}
			return out;
		},
		[](IntCmp& c) {
			std::string out = c.num_type == IntType::i32 ? "i32.": "i64.";
			switch(c.op_kind) {
				case IntCmp::Kind::Eq:
					out += "eq";
					break;
				case IntCmp::Kind::Ne:
					out += "ne";
					break;
				case IntCmp::Kind::LtU:
					out += "lt_u";
					break;
				case IntCmp::Kind::GtU:
					out += "gt_u";
					break;
				case IntCmp::Kind::LeU:
					out += "le_u";
					break;
				case IntCmp::Kind::GeU:
					out += "ge_u";
					break;
				case IntCmp::Kind::LtS:
					out += "lt_s";
					break;
				case IntCmp::Kind::GtS:
					out += "gt_s";
					break;
				case IntCmp::Kind::LeS:
					out += "le_s";
					break;
				case IntCmp::Kind::GeS:
					out += "ge_s";
					break;
			}
			return out;
		},
		[](BinaryBitwise& b) { 
			std::string out = b.num_type == IntType::i32 ? "i32.": "i64.";
			switch(b.op_kind) {
				case BinaryBitwise::Kind::And:
					out += "and";
					break;
				case BinaryBitwise::Kind::Or:
					out += "or";
					break;
				case BinaryBitwise::Kind::Xor:
					out += "xor";
					break;
				case BinaryBitwise::Kind::Shl:
					out += "shl";
					break;
				case BinaryBitwise::Kind::ShrU:
					out += "shru";
					break;
				case BinaryBitwise::Kind::ShrS:
					out += "shrs";
					break;
			}
			return out;

		},
		[](UnaryBitwise& b) { 
			std::string out = b.num_type == IntType::i32 ? "i32.": "i64.";
			switch(b.op_kind) {
				case UnaryBitwise::Kind::Not:
					out += "not";
					break;
			}
			return out;

		},
		[](Scope& s) { 
			std::string out = "";
			switch(s.kind) {
				case Scope::Kind::Block:
					out = "block";
					break;
				case Scope::Kind::Loop:
					out = "loop";
					break;
				case Scope::Kind::If: // TODO: add else block too
					out = "if";
					break;
			}
			return out;
		},
		[](End&) { return std::string("end"); },
		[](Return&) { return std::string("return"); },
		[](Br& b) { return "br " + std::to_string(b.index); },
		[](Call& c) { return "call " + std::to_string(c.index); },
		[](Local& l) { 
			std::string out = "local.";
			switch(l.kind) {
				case Local::Kind::Get:
					out += "get";
					break;
				case Local::Kind::Set:
					out += "set";
					break;
				case Local::Kind::Tee:
					out += "tee";
					break;
			}
			out += " " + std::to_string(l.index);
			return out;
		}
	};

	return std::visit(visitor,i);
}
