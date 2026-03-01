#include"vm.hpp"
#include<stdexcept>
#include"errors.hpp"

bool ControlFrame::is_block() {
	return std::holds_alternative<Block>(this->inner);
}

bool ControlFrame::is_activation_record() {
	return std::holds_alternative<ActivationRecord>(this->inner);
}

std::optional<size_t> ControlFrame::get_branch_target() {
	if(this->is_block()) {
		Block b = std::get<Block>(this->inner);
		if(b.br_action == BrAction::JumpToEnd) return b.info.block_end;
		return b.info.block_start;
	}

	return std::get<ActivationRecord>(this->inner).return_to;
}

std::optional<Block> ControlFrame::get_block() { 
	if(!this->is_block()) return {};
	return std::get<Block>(this->inner);
}
std::optional<ActivationRecord> ControlFrame::get_activation_record() {
	if(!this->is_activation_record()) return {};
	return std::get<ActivationRecord>(this->inner);
}

std::optional<ValueType> ControlFrame::get_return_type() {
	if(this->is_activation_record()) return this->get_activation_record().value().get_return_type();
	return this->get_block().value().info.return_type;
}

VM::VM() {
	this->stack = {};
}

void VM::push(Value v) {
	this->stack.push_back(v);
}

std::optional<Value> VM::pop() {
	if(!this->control_frames.empty()) {
		auto cf = this->control_frames.back();
		size_t bp = cf.initial_sp;
		if(this->stack.size() < bp) 
			throw StackUnderflowError();
	}

	if(this->stack.empty()) throw StackUnderflowError();
	Value top = this->stack.back();
	this->stack.pop_back();
	return top;
}

void VM::load(const std::vector<Instruction>& instrs) {
	instructions = instrs;
	ip = 0;
}

struct ArithemticVisitor {
	Arithmetic::Kind op_kind;

	template <typename T, typename U>
	Value operator()(const T& a, const U& b) {
		if constexpr (!std::is_same_v<T,U>) {
			throw std::runtime_error("Type mismatch");
		}
		switch(op_kind) {
			case Arithmetic::Kind::Add: return a + b;
			case Arithmetic::Kind::Sub: return a - b;
			case Arithmetic::Kind::Mul: return a * b;
		}
		throw std::runtime_error("unreachable");
	}
};

struct CmpVisitor {
	Cmp::Kind op_kind;

	template <typename T, typename U>
	Value operator()(const T& a, const U& b) {
		if constexpr (!std::is_same_v<T,U>) {
			throw std::runtime_error("Type mismatch");
		}
		switch(op_kind) {
			case Cmp::Kind::Eq: return a == b;
			case Cmp::Kind::Ne: return a != b;
			case Cmp::Kind::Lt: return a < b;
			case Cmp::Kind::Gt: return a > b;
			case Cmp::Kind::Le: return a <= b;
			case Cmp::Kind::Ge: return a >= b;
		}
		throw std::runtime_error("unreachable");
	}
};

struct BitwiseVisitor {
	Bitwise::Kind op_kind;


	template<typename T, typename U>
	Value operator()(const T& a, const U& b) {
		(void)a;
		(void)b;
		throw std::runtime_error("Type mismatch");
		return {};
	}


	template <typename T>
	Value operator()(const T& a, const T& b) {
		if constexpr (!std::is_same_v<int32_t,T> && !std::is_same_v<int64_t,T>) {
			throw std::runtime_error("Expected either i32 or i64 for bitwise operation");
		}else {

		ValueType vt = to_value_type(a); 

		switch(op_kind) {
			case Bitwise::Kind::And: return a & b;
			case Bitwise::Kind::Or:  return a | b;
			case Bitwise::Kind::Not: throw std::runtime_error("Must be handled outside visitor"); // This errors because all the other variants take in two arguments and this takes in only one.
													  // Could be made better.
			case Bitwise::Kind::Xor: return a ^ b;
			case Bitwise::Kind::Shl: 
					     if(vt == ValueType::i32) {
						     int32_t shift = a;
						     int32_t value = b;
						     uint32_t amt = static_cast<uint32_t>(shift) & 31u;		//masks the output to prevent UB, by returning the 5 LSBs
						     return static_cast<int32_t>(static_cast<uint32_t>(value) << amt);

					     }else if(vt == ValueType::i64){
						     // TODO
					     }
					     throw std::runtime_error("unreachable");

			case Bitwise::Kind::ShrU:
					     if(vt == ValueType::i32) {
						     int32_t shift = a;
						     int32_t value = b;
						     uint32_t amt = static_cast<uint32_t>(shift) & 31u;
						     return static_cast<int32_t>(static_cast<uint32_t>(value) >> amt);

					     }else if(vt == ValueType::i64){
						     // TODO
					     }
					     throw std::runtime_error("unreachable");
			case Bitwise::Kind::ShrS:
					     if(vt == ValueType::i32) {
						     int32_t shift = a;
						     int32_t value = b;
						     uint32_t amt = static_cast<uint32_t>(shift) & 31u;
						     return static_cast<int32_t>(value >> amt);
					     }else if(vt == ValueType::i64){
						     // TODO
					     }
					     throw std::runtime_error("unreachable");
		}
		}
		throw std::runtime_error("unreachable");
	}
};

bool VM::run_instr(const Instruction& instr) {
	const auto visitor = overloads {
		[&](const Nop&) { return true;},
		[&](const Unreachable&) { return false;},
		[&](const LoadConst& i) {
			this->push(i.value);
			return true;
		},
		[&](const Arithmetic& a_instr) { 
			ValueType type = a_instr.num_type;	
			this->expect_stack({type,type});

			Value v1 = this->pop().value();
			Value v2 = this->pop().value();

			this->push(std::visit(ArithemticVisitor{a_instr.op_kind},v1,v2));
			return true;
		},
		[&](const Cmp& instr) { 
			ValueType type = instr.num_type;	
			this->expect_stack({type,type});

			Value v1 = this->pop().value();
			Value v2 = this->pop().value();

			this->push(std::visit(CmpVisitor{instr.op_kind},v1,v2));
			return true;

		},
		[&](const Bitwise& instr) { 
			ValueType type = instr.num_type == Bitwise::VType::i32 ? ValueType::i32 : ValueType::i64;	
			
			// handle this a little differently because all the other instruction kinds take in two arguments while this guy takes in only one 
			if(instr.op_kind == Bitwise::Kind::Not) {
				this->expect_stack({type});
				Value v1 = this->pop().value();
				if(std::holds_alternative<int32_t>(v1)) {
					this->push(~std::get<int32_t>(v1));
					return true;
				}
				this->push(~std::get<int32_t>(v1));
				return true;
			}

			this->expect_stack({type,type});

			Value v1 = this->pop().value();
			Value v2 = this->pop().value();

			this->push(std::visit(BitwiseVisitor{instr.op_kind},v1,v2));
			return true;

		},
		[&](const Scope& scope) { 
			Block b= Block{.br_action = BrAction::JumpToEnd, .info = scope.info};

			switch(scope.kind) {
			case Scope::Kind::Block:
				 b.br_action = BrAction::JumpToEnd;
				break;
			case Scope::Kind::Loop:
				 b.br_action = BrAction::JumpToStart;
				break;
			case Scope::Kind::If:
				this->expect_stack({ValueType::i32});

				int32_t v = std::get<int32_t>(this->pop().value());
				if(v > 0) {
					b.info = scope.info;
				}else if(scope.else_info.has_value()) {
					b.info = scope.else_info.value();
				}else {
					this->ip = scope.info.block_end; // just exit if condition was false and there wass no other else branch to go out
					return true;
				}

				b.info.block_end = scope.if_else_end.value_or(scope.info.block_end); // so that we jump out of the if-else chain when we are done with them
			    
				// We use a regular block for this, because when you br out of an if block, you go the end of block
				b.br_action = BrAction::JumpToEnd;
				break;
			}
			ControlFrame cf = ControlFrame(b,this->stack.size());
			this->control_frames.push_back(cf);
			this->ip = b.info.block_start;

			return true;
		},
		[&](const End&) {  
			if(this->control_frames.empty())  {
				throw UnexpectedInstruction(instr,this->ip);
			}
			auto cf = this->control_frames.back(); 

			std::optional<ValueType> expected_return_type = cf.get_return_type();
			std::optional<Value> return_val = {};
			if(expected_return_type.has_value()) {
				this->expect_stack({expected_return_type.value()});
				return_val = this->pop();
			}

			size_t cur_sp = this->stack.size();
			size_t prev_sp = cf.initial_sp;
			if(prev_sp > cur_sp)
				StackUnderflowError();
			size_t n_to_pop =  cur_sp - prev_sp; 
			for(size_t i=0;i<n_to_pop;i++) this->pop();
			if(return_val.has_value()) this->push(return_val.value());

			this->ip = cf.get_branch_target().value();
			this->control_frames.pop_back(); 

			return true;

		},
		[&](const Br& instr) {  
			size_t bk = instr.index;

			if(this->control_frames.size() <=  bk) 
				throw StackUnderflowError();

			std::optional<Value> return_value = {};

			ControlFrame req_cf = this->control_frames.at(this->control_frames.size()-bk-1);
			if(!req_cf.is_block()) throw UnexpectedInstruction(instr,this->ip);

			Block required_block = req_cf.get_block().value();
			size_t loc = req_cf.get_branch_target().value();	
			// If we're jumping out of a loop, then we do not want to break out of the loop, just rerun it
			if(required_block.br_action == BrAction::JumpToStart) {
				this->ip = loc;
				return true;
			}

			for(size_t i = 0;i<=bk;i++) {
				ControlFrame cf = this->control_frames.back();
				
				if(!cf.is_block()) throw UnexpectedInstruction(instr,this->ip);

				Block blk = cf.get_block().value();

				std::optional<ValueType> expected_return_type = blk.info.return_type;
				size_t items_to_pop = this->stack.size() - cf.initial_sp;

				if(expected_return_type.has_value() && !return_value.has_value() && !this->stack.empty()) {
					return_value = this->pop();
					items_to_pop--;
				}else if(!expected_return_type.has_value()) {
					return_value = {};
				}

				for(size_t j =0;j<items_to_pop;j++) this->pop();

				this->control_frames.pop_back();
			}
			this->ip = loc;
			if(return_value.has_value()) this->push(return_value.value());
			return true;
		},
		[&](const Return&) { 
			// essentially: loop through control_frames from the back. propagate any return values if there are. Stop when you reach an activation record

			if(this->control_frames.empty()) return false;
			ControlFrame top_cf = this->control_frames.back();
			std::optional<ValueType> return_type = top_cf.get_return_type();
			std::optional<Value> return_val;
			if(return_type.has_value()) {
				this->expect_stack({return_type.value()});
				return_val = this->pop();
			}

			while(true) {
				ControlFrame cf = this->control_frames.back();
				this->control_frames.pop_back();
				size_t cur_sp = this->stack.size();
				size_t prev_sp = cf.initial_sp;
				size_t n_to_pop = cur_sp - prev_sp;
				for(size_t i = 0;i<n_to_pop;i++) this->pop();	

				if(!cf.is_activation_record()) continue;

				// If it is a activation record
				if(return_type.has_value() && cf.get_return_type().has_value()) {
					this->push(return_val.value());
					this->expect_stack({cf.get_return_type().value()});
				}
				std::optional<size_t> return_to = cf.get_branch_target();
				// We terminate the program here because the only (legal) way a function can be called without having a return address is if it was from external sources or if it was a start function, which are legal and aren't expected to error.
				if(!return_to.has_value()) return false;

				this->ip = return_to.value();

				break;
			}
			return true;

		},
		[&](const Call& instr) {  
			FunctionInfo& fn_info = this->functions.at(instr.index);

			this->expect_stack(fn_info.args);

			BlockInfo fn_blk_info = fn_info.block_info;
			size_t return_to_index = this->ip;

			// Loading arguments supplied to local variables
			std::vector<Value> locals = {};
			for(auto it = this->stack.rbegin(); locals.size() != fn_info.args.size(); it++) {
				locals.push_back(*it);
			}

			ActivationRecord a = ActivationRecord(fn_info, locals);
			a.return_to = return_to_index;
			ControlFrame cf = ControlFrame(a,this->stack.size());
			this->control_frames.push_back(cf);	

			this->ip = fn_blk_info.block_start;

			return true;
		}
	};
	return std::visit(visitor,instr);
}

void VM::run() {
	while (ip < instructions.size()) {
		try {
			if (!run_instr(instructions[ip])) {
				break;
			}
			ip++;
		} catch (...) {
			throw VMError(std::string("Error occured while running instruction `" + to_string(instructions[ip])  + "` at index "  + std::to_string(ip)),std::current_exception());
		}
	}
}


void VM::expect_stack(std::vector<ValueType> expected_values) {

	std::vector<Value> cur_stack = this->stack;
	if(!this->control_frames.empty()) {
		ControlFrame cf = this->control_frames.back();	
		if(cf.initial_sp > this->stack.size()) {
			throw VMError("FATAL: Stack overflowed."); // should almost never reach here
		}
		cur_stack = std::vector<Value>(this->stack.begin()+cf.initial_sp,this->stack.end());
		if(this->stack.size() - cf.initial_sp < expected_values.size())  {
			throw ExpectStackError(expected_values,cur_stack);
		}
	}else if(this->stack.size() < expected_values.size())  {

		throw ExpectStackError(expected_values,this->stack);
	}

	auto s_elm = this->stack.rbegin();
	for(auto elm = expected_values.rbegin(); elm != expected_values.rend(); elm++) {
		Value &value = *s_elm;
		s_elm++;
		switch(*elm) {
			case ValueType::i32: if(!std::holds_alternative<int32_t>(value)) throw ExpectStackError(expected_values,cur_stack); break;
			case ValueType::i64: if(!std::holds_alternative<int64_t>(value)) throw ExpectStackError(expected_values,cur_stack); break;
			case ValueType::f32: if(!std::holds_alternative<float>(value))   throw ExpectStackError(expected_values,cur_stack); break;
			case ValueType::f64: if(!std::holds_alternative<double>(value))  throw ExpectStackError(expected_values,cur_stack); break;
		}
	}

}

void VM::expect_stack_exact(std::vector<ValueType> expected_values) {
	if(!this->control_frames.empty()) {
		ControlFrame cf = this->control_frames.back();
		if(cf.initial_sp > this->stack.size() || this->stack.size() - cf.initial_sp != expected_values.size()) {
			auto cur_stack = std::vector<Value>(this->stack.begin()+cf.initial_sp,this->stack.end());
			throw ExpectStackError(expected_values,cur_stack);
		}
	}else if(this->stack.size() != expected_values.size()) throw ExpectStackError(expected_values,this->stack);
	this->expect_stack(expected_values);
}


size_t VM::register_function(FunctionInfo f) {
	this->functions.push_back(f);	
	return this->functions.size() - 1;
}
