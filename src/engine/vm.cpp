#include"vm.hpp"
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

bool VM::run_instr(const Instruction& instr) {

	switch (instr.kind) {
		case InstrKind::NOP: return true;
		case InstrKind::UNREACHABLE: return false;
		case InstrKind::I32_CONST: {
						   InstrArgs args = instr.args.value();
						   int32_t value = std::get<int32_t>(args.value);
						   push(value);
						   return true;
					   }

		case InstrKind::I32_ADD: {
						 this->expect_stack({ValueType::i32,ValueType::i32});	
						 auto b = std::get<int32_t>(*pop());
						 auto a = std::get<int32_t>(*pop());
						 push(a + b);
						 return true;
					 }

		case InstrKind::I32_SUB: {
						 this->expect_stack({ValueType::i32,ValueType::i32});	
						 auto b = std::get<int32_t>(*pop());
						 auto a = std::get<int32_t>(*pop());
						 push(a - b);
						 return true;
					 }

		case InstrKind::I32_MUL: {
						 this->expect_stack({ValueType::i32,ValueType::i32});	
						 auto b = std::get<int32_t>(*pop());
						 auto a = std::get<int32_t>(*pop());
						 push(a * b);
						 return true;
					 }

		case InstrKind::I32_EQ: {
						this->expect_stack({ValueType::i32,ValueType::i32});
						auto b = std::get<int32_t>(*pop());
						auto a = std::get<int32_t>(*pop());
						push(static_cast<int32_t>(a == b));
						return true;
					}

		case InstrKind::I32_AND: {
						 this->expect_stack({ValueType::i32,ValueType::i32});
						 auto b = std::get<int32_t>(*pop());
						 auto a = std::get<int32_t>(*pop());
						 push(a & b);
						 return true;
					 }

		case InstrKind::I32_OR: {
						this->expect_stack({ValueType::i32,ValueType::i32});
						auto b = std::get<int32_t>(*pop());
						auto a = std::get<int32_t>(*pop());
						push(a | b);
						return true;
					}

		case InstrKind::I32_XOR: {
						 this->expect_stack({ValueType::i32,ValueType::i32});
						 auto b = std::get<int32_t>(*pop());
						 auto a = std::get<int32_t>(*pop());
						 push(a ^ b);
						 return true;
					 }

		case InstrKind::I32_NOT: {
						 this->expect_stack({ValueType::i32});
						 auto a = std::get<int32_t>(*pop());
						 push(~a);
						 return true;
					 }

		case InstrKind::I32_SHL: {
						 this->expect_stack({ValueType::i32,ValueType::i32});
						 auto shift = std::get<int32_t>(*pop());
						 auto value = std::get<int32_t>(*pop());
						 uint32_t amt = static_cast<uint32_t>(shift) & 31u;		//masks the output to prevent UB, by returning the 5 LSBs
						 push(static_cast<int32_t>(static_cast<uint32_t>(value) << amt));
						 return true;
					 }

		case InstrKind::I32_SHR_U: {
						   this->expect_stack({ValueType::i32,ValueType::i32});
						   auto shift = std::get<int32_t>(*pop());
						   auto value = std::get<int32_t>(*pop());
						   uint32_t amt = static_cast<uint32_t>(shift) & 31u;
						   push(static_cast<int32_t>(static_cast<uint32_t>(value) >> amt));
						   return true;
					   }
		case InstrKind::I32_SHR_S: {
						   this->expect_stack({ValueType::i32,ValueType::i32});
						   auto shift = std::get<int32_t>(*pop());
						   auto value = std::get<int32_t>(*pop());
						   uint32_t amt = static_cast<uint32_t>(shift) & 31u;
						   push(static_cast<int32_t>(value >> amt));
						   return true;
					   }

		case InstrKind::END: {
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
					     for(int i=0;i<n_to_pop;i++) this->pop();
					     if(return_val.has_value()) this->push(return_val.value());

					     this->ip = cf.get_branch_target().value();
					     this->control_frames.pop_back(); 

					     return true;
				     }

		case InstrKind::BLOCK: {
					       InstrArgs args = instr.args.value();
					       auto bk = args.block;
					       Block b = {.br_action = BrAction::JumpToEnd,.info = bk};
					       auto cf = ControlFrame(b,this->stack.size());
					       this->control_frames.push_back(cf);
					       return true;
				       }

		case InstrKind::LOOP: {
					      InstrArgs args = instr.args.value();
					      auto bk = args.block;
					      Block b = {.br_action = BrAction::JumpToStart,.info = bk};
					      this->control_frames.push_back(ControlFrame(b,this->stack.size()));
					      return true;
				      }

		case InstrKind::BR: {
					    InstrArgs args = instr.args.value();
					    size_t bk = args.index;

					    if(this->control_frames.size() <=  bk) 
						    throw StackUnderflowError();

					    size_t loc;	
					    std::optional<Value> return_value = {};
					    for(int i = 0;i<=bk;i++) {
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

						    loc = cf.get_branch_target().value();
						    this->control_frames.pop_back();
					    }
					    this->ip = loc;
					    if(return_value.has_value()) this->push(return_value.value());
					    return true;
				    }

		case InstrKind::IF: {
					    this->expect_stack({ValueType::i32});	
					    InstrArgs args = instr.args.value();
					    BlockInfo if_blk_info = args.if_.if_block_info; 
					    std::optional<BlockInfo> else_blk_info_opt = args.if_.else_block_info; 

					    Value val = this->pop().value(); 
					    int32_t v = std::get<int32_t>(val);  

					    BlockInfo blk_info; 

					    if(v==1) {
						    blk_info = if_blk_info;
					    }else if(else_blk_info_opt.has_value()) {
						    blk_info = else_blk_info_opt.value();
					    }else {
						    this->ip = args.if_.end_instr; // just exit if condition was false and there wass no other else branch to go out
						    return true;
					    }

					    blk_info.block_end = args.if_.end_instr; // so that we jump out of the if-else chain when we are done with them


					    // We use a regular block for this, because when you br out of an if block, you go the end of block
					    Block b = Block{.br_action = BrAction::JumpToEnd,.info = blk_info};
					    this->control_frames.push_back(ControlFrame(b,this->stack.size()));
					    this->ip = blk_info.block_start; 
					    return true;
				    }

		case InstrKind::CALL: {
					      InstrArgs args = instr.args.value();
					      FunctionInfo& fn_info = this->functions.at(args.index);

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

		case InstrKind::RETURN: {
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
							for(int i = 0;i<n_to_pop;i++) this->pop();	

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
					}

	}
	return true;
}

void VM::run() {
	while (ip < instructions.size()) {
		try {
			if (!run_instr(instructions[ip])) {
				break;
			}
			ip++;
		} catch (...) {
			throw VMError(std::string("Error occured while running instruction `" + instructions[ip].to_string()  + "` at index "  + std::to_string(ip)),std::current_exception());
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
