#include<cstring> //for std::memcpy()
#include<utility> //for std::pair<>
#include "parser/codeparsing.hpp"

std::vector<Instruction> parse_code(std::span<const uint8_t> data, size_t& offset) 
{
	size_t codeSize = data.size();

	Instr opcode;
	InstrCategory category;
	std::vector<Instruction> instructions;
    size_t instrvectorEnd = 0;
    Instruction instr;

    std::vector<std::pair<Instruction*, uint8_t>> scope_stack{};
    std::pair<Instruction*, uint8_t> scope_stack_item;
        //the uint8_t in the pair is to mark whether an else block is being parsed currently or not. Algo is as follows
        /*
        On reading a Block, Loop or If: 
            1. create an instruction of type Scope
            2. push instr into instruction vector
            3. fill instr.info.block_start with current instruction index (instructions.size() - 1)
            4. Push <&instr, 0> into scope_stack
        On reading an Else:
            1. Push End{} into instruction vector
            2. Pop from top of scope_stack. If the code is valid (i.e. no dangling else), instr->kind must be Scope::Kind::If. If this is not true, reject code
            3. write current instruction index into instr->info.block_end
            4. write current instruction index + 1 into instr->else_info.block_start
            5. push the same instruction pointer as <instr, 1> into scope_stack since you are now parsing an else part
        On reading an End:
            1. Push End{} into instruction vector
            2. Pop from top of scope_stack
            3. If uint8_t == 0
                The block you just finished parsing is not an else block. 
                1. Write current instruction index to instr->info.block_end
                2. Discard pointer
               else (i.e. uint8_t == 1)
                You have just finished parsing an else block. 
                1. Write current instruction index into instr->else_info.block_end
                2. Discard pointer
        */
    
	while (offset < codeSize) 
    {
		opcode = static_cast<Instr>(data[offset]);
        std::cout << "Opcode in hex is: " << std::hex << int(data[offset]) << std::endl;
		++offset;
		category = Instr_to_Category.at(opcode);
        print_string_of_opcode(opcode);
        std::cout << "Instr Category number of opcode: " << static_cast<int>(category) << std::endl; 
		switch (category) {
			case InstrCategory::UNKNOWN: 
			{
				std::cout << "Unimplemented instruction: ";
				print_string_of_opcode(opcode);	
				std::cout << std::endl;
				break;
			}

            //Scope related opcodes - parsing these involves interacting with the scope_stack, so they are grouped here
            case InstrCategory::Scope:
            {
                instructions.push_back(parse_Scope(data, codeSize, offset, opcode, instrvectorEnd));
                scope_stack_item.first = &instructions[instrvectorEnd];
                scope_stack_item.second = 0;
                scope_stack.push_back(scope_stack_item);
                ++instrvectorEnd;
                break;
            }
            case InstrCategory::ELSE:
            {
                instructions.push_back(End{});
                if (scope_stack.empty()) 
                {
                    std::cout << "Error, dangling else detected. Returning empty instruction vector." << std::endl;
                    return std::vector<Instruction>();
                }
                else 
                {
                    scope_stack_item = scope_stack.back();
                    scope_stack.pop_back();
                    if (std::get<Scope>(*(scope_stack_item.first)).kind != Scope::Kind::If) 
                    {
                        std::cout << "Error, dangling else detected. Returning empty instruction vector." << std::endl;
                        return std::vector<Instruction>();
                    }
                    else 
                    {
                        std::get<Scope>(*(scope_stack_item.first)).info.block_end = instrvectorEnd;
                        std::get<Scope>(*(scope_stack_item.first)).else_info = BlockInfo{.block_start = instrvectorEnd + 1, //else block starts in the next instruction
                            .block_end = 0, //fill in block_end with 0 for now, it will be filled in with proper value later
                            .return_type = std::nullopt}; //else block never has a return type of its own, it shares with if block
                        scope_stack_item.second = 1;
                        scope_stack.push_back(scope_stack_item);
                    }
                }
                ++instrvectorEnd;
                break;
            }
			case InstrCategory::End: 
			{
				instructions.push_back(End{});
                if (scope_stack.empty()) 
                {
                    //assuming the code is valid, this means we've reached the end of the entire block of the code. 
                    //Hence, return instruction vector.
                    return instructions;
                }
                else 
                {
                    scope_stack_item = scope_stack.back();
                    scope_stack.pop_back();
                    if (scope_stack_item.second == 1) 
                    {
                        std::get<Scope>(*(scope_stack_item.first)).else_info.value().block_end = instrvectorEnd;
                    }
                    else 
                    {
                        std::get<Scope>(*(scope_stack_item.first)).info.block_end = instrvectorEnd;
                    }
                    ++instrvectorEnd;
                }
				break;
			}

            //The opcodes take no arguments from the wasm binary, and their corresponding structs are empty
			case InstrCategory::Nop: 
			{
				instructions.push_back(Nop{});
                ++instrvectorEnd;
				break;
			}
			case InstrCategory::Unreachable:
			{
				instructions.push_back(Unreachable{});
                ++instrvectorEnd;
                break;
			}
			case InstrCategory::Return: 
			{
				instructions.push_back(Return{});
                ++instrvectorEnd;
				break;
			}

            //These opcodes also take no arguments from the wasm binary, but they do have elements inside their structs
			case InstrCategory::IntArithmetic: 
			{
				instructions.push_back(parse_IntArithmetic(opcode));
                ++instrvectorEnd;
				break;
			}
			case InstrCategory::FloatArithmetic:
			{
				instructions.push_back(parse_FloatArithmetic(opcode));
                ++instrvectorEnd;
                break;
			}
			case InstrCategory::UnaryInt:
			{
				instructions.push_back(parse_UnaryInt(opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::UnaryFloat:
			{
				instructions.push_back(parse_UnaryFloat(opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::BinaryFloat:
			{
				instructions.push_back(parse_BinaryFloat(opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::BinaryBitwise:
			{
				instructions.push_back(parse_BinaryBitwise(opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::IntCmp:
			{
				instructions.push_back(parse_IntCmp(opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::FloatCmp:
			{
				instructions.push_back(parse_FloatCmp(opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::IntConverters:
            {
                instructions.push_back(parse_IntConverters(opcode));
                ++instrvectorEnd;
                break;
            }
            case InstrCategory::FloatConverters:
            {
                instructions.push_back(parse_FloatConverters(opcode));
                ++instrvectorEnd;
                break;
            }
            case InstrCategory::FloatToIntTrunc:
            {
                instructions.push_back(parse_FloatToIntTrunc(opcode));
                ++instrvectorEnd;
                break;
            }
            case InstrCategory::IntToFloat:
            {
                instructions.push_back(parse_IntToFloat(opcode));
                ++instrvectorEnd;
                break;
            }
            case InstrCategory::ReinterpretBits:
            {
                instructions.push_back(parse_ReinterpretBits(opcode));
                ++instrvectorEnd;
                break;
            }

            //These opcodes do take arguments from the wasm binary
            case InstrCategory::Call:
            {
                size_t arg = leb128_decode(data, codeSize, offset);
                instructions.push_back(Call{.index = arg});
                ++instrvectorEnd;
                break;
            }
            case InstrCategory::Br:
            {
                instructions.push_back(parse_Br(data, codeSize, offset, opcode));
                ++instrvectorEnd;
                break;
            }
            case InstrCategory::LoadConst:
			{
				instructions.push_back(parse_LoadConst(data, codeSize, offset, opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::Local:
			{
				instructions.push_back(parse_Local(data, codeSize, offset, opcode));
                ++instrvectorEnd;
                break;
			}
            case InstrCategory::Global:
			{
				instructions.push_back(parse_Global(data, codeSize, offset, opcode));
                ++instrvectorEnd;
                break;
			}

            default: //This should never happen, so just returning an empty instruction vector 
            {
                return std::vector<Instruction>{};
            }
		}
	}
    
    return instructions;
}

Instruction parse_IntArithmetic(Instr opcode)
{
	IntArithmetic instr{};
	switch (opcode) {
		case Instr::I32_ADD: 
		{
			instr.op_kind = IntArithmetic::Kind::Add;
			instr.num_type = IntType::i32;
			break;
		}
		case Instr::I32_SUB:
		{
			instr.op_kind = IntArithmetic::Kind::Sub;
			instr.num_type = IntType::i32;
			break;			
		}
		case Instr::I32_MUL:
		{
			instr.op_kind = IntArithmetic::Kind::Mul;
			instr.num_type = IntType::i32;
			break;			
		}
		case Instr::I32_DIV_S:
		{
			instr.op_kind = IntArithmetic::Kind::DivS;
			instr.num_type = IntType::i32;
			break;
		}
		case Instr::I32_DIV_U:
		{
			instr.op_kind = IntArithmetic::Kind::DivU;
			instr.num_type = IntType::i32;
			break;			
		}
		case Instr::I32_REM_S:
		{
			instr.op_kind = IntArithmetic::Kind::RemS;
			instr.num_type = IntType::i32;
			break;			
		}
		case Instr::I32_REM_U:
		{
			instr.op_kind = IntArithmetic::Kind::RemU;
			instr.num_type = IntType::i32;
			break;
		}
		case Instr::I64_ADD:
		{
			instr.op_kind = IntArithmetic::Kind::Add;
			instr.num_type = IntType::i64;
			break;
		}
		case Instr::I64_SUB:
		{
			instr.op_kind = IntArithmetic::Kind::Sub;
			instr.num_type = IntType::i64;
			break;
		}
		case Instr::I64_MUL:
		{
			instr.op_kind = IntArithmetic::Kind::Mul;
			instr.num_type = IntType::i64;
			break;
		}
		case Instr::I64_DIV_S:
		{
			instr.op_kind = IntArithmetic::Kind::DivS;
			instr.num_type = IntType::i64;
			break;
		}
		case Instr::I64_DIV_U:
		{
			instr.op_kind = IntArithmetic::Kind::DivU;
			instr.num_type = IntType::i64;
			break;
		}
		case Instr::I64_REM_S:
		{
			instr.op_kind = IntArithmetic::Kind::RemS;
			instr.num_type = IntType::i64;
			break;
		}
		case Instr::I64_REM_U:
		{
			instr.op_kind = IntArithmetic::Kind::RemU;
			instr.num_type = IntType::i64;
			break;
		}
        default: //this should never happen so blindly break
        {
            break;
        }
	}

    return Instruction{std::in_place_type<IntArithmetic>, instr};
}

Instruction parse_FloatArithmetic(Instr opcode)
{
    FloatArithmetic instr{};
	switch (opcode) 
    {
		case Instr::F32_ADD: 
		{
			instr.op_kind = FloatArithmetic::Kind::Add;
			instr.num_type = FloatType::f32;
			break;
		}
		case Instr::F32_SUB:
		{
			instr.op_kind = FloatArithmetic::Kind::Sub;
			instr.num_type = FloatType::f32;
			break;			
		}
		case Instr::F32_MUL:
		{
			instr.op_kind = FloatArithmetic::Kind::Mul;
			instr.num_type = FloatType::f32;
			break;			
		}
		case Instr::F32_DIV:
		{
			instr.op_kind = FloatArithmetic::Kind::Div;
			instr.num_type = FloatType::f32;
			break;
		}
		case Instr::F64_ADD: 
		{
			instr.op_kind = FloatArithmetic::Kind::Add;
			instr.num_type = FloatType::f64;
			break;
		}
		case Instr::F64_SUB:
		{
			instr.op_kind = FloatArithmetic::Kind::Sub;
			instr.num_type = FloatType::f64;
			break;			
		}
		case Instr::F64_MUL:
		{
			instr.op_kind = FloatArithmetic::Kind::Mul;
			instr.num_type = FloatType::f64;
			break;			
		}
		case Instr::F64_DIV:
		{
			instr.op_kind = FloatArithmetic::Kind::Div;
			instr.num_type = FloatType::f64;
			break;
		}
        default: //this should never happen so blindly break
        {
            break;
        }
	}

    return Instruction{std::in_place_type<FloatArithmetic>, instr};
}

Instruction parse_UnaryInt(Instr opcode)
{
    UnaryInt instr{};
    switch (opcode) 
    {
        case Instr::I32_CLZ:
        {
            instr.op_kind = UnaryInt::Kind::Clz;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_CTZ:
        {
            instr.op_kind = UnaryInt::Kind::Ctz;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_POPCNT:
        {
            instr.op_kind = UnaryInt::Kind::Popcnt;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I64_CLZ:
        {
            instr.op_kind = UnaryInt::Kind::Clz;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_CTZ:
        {
            instr.op_kind = UnaryInt::Kind::Ctz;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_POPCNT:
        {
            instr.op_kind = UnaryInt::Kind::Popcnt;
            instr.num_type = IntType::i64;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<UnaryInt>, instr};
}

Instruction parse_UnaryFloat(Instr opcode)
{
    UnaryFloat instr{};
    switch (opcode) 
    {
        case Instr::F32_ABS:
        {
            instr.op_kind = UnaryFloat::Kind::Abs;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_NEG:
        {
            instr.op_kind = UnaryFloat::Kind::Neg;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_CEIL:
        {
            instr.op_kind = UnaryFloat::Kind::Ceil; //They've made a typo in the engine branch, they've put Ciel
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_FLOOR:
        {
            instr.op_kind = UnaryFloat::Kind::Floor;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_TRUNC:
        {
            instr.op_kind = UnaryFloat::Kind::Trunc;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_NEAREST:
        {
            instr.op_kind = UnaryFloat::Kind::Nearest;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_SQRT:
        {
            instr.op_kind = UnaryFloat::Kind::Sqrt;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F64_ABS:
        {
            instr.op_kind = UnaryFloat::Kind::Abs;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_NEG:
        {
            instr.op_kind = UnaryFloat::Kind::Neg;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_CEIL:
        {
            instr.op_kind = UnaryFloat::Kind::Ceil; //They've made a typo in the engine branch, they've put Ciel
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_FLOOR:
        {
            instr.op_kind = UnaryFloat::Kind::Floor;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_TRUNC:
        {
            instr.op_kind = UnaryFloat::Kind::Trunc;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_NEAREST:
        {
            instr.op_kind = UnaryFloat::Kind::Nearest;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_SQRT:
        {
            instr.op_kind = UnaryFloat::Kind::Sqrt;
            instr.num_type = FloatType::f64;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<UnaryFloat>, instr};
}

Instruction parse_BinaryFloat(Instr opcode)
{
    BinaryFloat instr{};
    switch (opcode) 
    {
        case Instr::F32_MIN:
        {
            instr.op_kind = BinaryFloat::Kind::Min;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_MAX:
        {
            instr.op_kind = BinaryFloat::Kind::Max;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_COPYSIGN:
        {
            instr.op_kind = BinaryFloat::Kind::CopySign;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_MIN:
        {
            instr.op_kind = BinaryFloat::Kind::Min;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_MAX:
        {
            instr.op_kind = BinaryFloat::Kind::Max;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_COPYSIGN:
        {
            instr.op_kind = BinaryFloat::Kind::CopySign;
            instr.num_type = FloatType::f64;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<BinaryFloat>, instr};
}

Instruction parse_BinaryBitwise(Instr opcode) //Only integers can perform these operations
{
    BinaryBitwise instr{};
    switch (opcode) 
    {
        case Instr::I32_AND:
        {
            instr.op_kind = BinaryBitwise::Kind::And;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_OR:
        {
            instr.op_kind = BinaryBitwise::Kind::Or;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_XOR:
        {
            instr.op_kind = BinaryBitwise::Kind::Xor;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_SHL:
        {
            instr.op_kind = BinaryBitwise::Kind::Shl;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_SHR_U:
        {
            instr.op_kind = BinaryBitwise::Kind::ShrU;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_SHR_S:
        {
            instr.op_kind = BinaryBitwise::Kind::ShrS;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I64_AND:
        {
            instr.op_kind = BinaryBitwise::Kind::And;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_OR:
        {
            instr.op_kind = BinaryBitwise::Kind::Or;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_XOR:
        {
            instr.op_kind = BinaryBitwise::Kind::Xor;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_SHL:
        {
            instr.op_kind = BinaryBitwise::Kind::Shl;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_SHR_U:
        {
            instr.op_kind = BinaryBitwise::Kind::ShrU;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_SHR_S:
        {
            instr.op_kind = BinaryBitwise::Kind::ShrS;
            instr.num_type = IntType::i64;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<BinaryBitwise>, instr};
}

Instruction parse_IntCmp(Instr opcode) //Only integers can perform these operations
{
    IntCmp instr{};
    switch (opcode)
    {
        case Instr::I32_EQ:
        {
            instr.op_kind = IntCmp::Kind::Eq;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_NE:
        {
            instr.op_kind = IntCmp::Kind::Ne;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_LT_U:
        {
            instr.op_kind = IntCmp::Kind::LtU;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_GT_U:
        {
            instr.op_kind = IntCmp::Kind::GtU;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_LE_U:
        {
            instr.op_kind = IntCmp::Kind::LeU;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_GE_U:
        {
            instr.op_kind = IntCmp::Kind::GeU;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_LT_S:
        {
            instr.op_kind = IntCmp::Kind::LtS;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_GT_S:
        {
            instr.op_kind = IntCmp::Kind::GtS;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_LE_S:
        {
            instr.op_kind = IntCmp::Kind::LeS;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I32_GE_S:
        {
            instr.op_kind = IntCmp::Kind::GeS;
            instr.num_type = IntType::i32;
            break;
        }
        case Instr::I64_EQ:
        {
            instr.op_kind = IntCmp::Kind::Eq;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_NE:
        {
            instr.op_kind = IntCmp::Kind::Ne;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_LT_U:
        {
            instr.op_kind = IntCmp::Kind::LtU;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_GT_U:
        {
            instr.op_kind = IntCmp::Kind::GtU;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_LE_U:
        {
            instr.op_kind = IntCmp::Kind::LeU;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_GE_U:
        {
            instr.op_kind = IntCmp::Kind::GeU;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_LT_S:
        {
            instr.op_kind = IntCmp::Kind::LtS;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_GT_S:
        {
            instr.op_kind = IntCmp::Kind::GtS;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_LE_S:
        {
            instr.op_kind = IntCmp::Kind::LeS;
            instr.num_type = IntType::i64;
            break;
        }
        case Instr::I64_GE_S:
        {
            instr.op_kind = IntCmp::Kind::GeS;
            instr.num_type = IntType::i64;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<IntCmp>, instr};
}

Instruction parse_FloatCmp(Instr opcode)
{
    FloatCmp instr{};
    switch (opcode)
    {
        case Instr::F32_EQ:
        {
            instr.op_kind = FloatCmp::Kind::Eq;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_NE:
        {
            instr.op_kind = FloatCmp::Kind::Ne;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_LT:
        {
            instr.op_kind = FloatCmp::Kind::Lt;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_GT:
        {
            instr.op_kind = FloatCmp::Kind::Gt;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_LE:
        {
            instr.op_kind = FloatCmp::Kind::Le;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F32_GE:
        {
            instr.op_kind = FloatCmp::Kind::Ge;
            instr.num_type = FloatType::f32;
            break;
        }
        case Instr::F64_EQ:
        {
            instr.op_kind = FloatCmp::Kind::Eq;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_NE:
        {
            instr.op_kind = FloatCmp::Kind::Ne;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_LT:
        {
            instr.op_kind = FloatCmp::Kind::Lt;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_GT:
        {
            instr.op_kind = FloatCmp::Kind::Gt;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_LE:
        {
            instr.op_kind = FloatCmp::Kind::Le;
            instr.num_type = FloatType::f64;
            break;
        }
        case Instr::F64_GE:
        {
            instr.op_kind = FloatCmp::Kind::Ge;
            instr.num_type = FloatType::f64;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<FloatCmp>, instr};
}

Instruction parse_IntConverters(Instr opcode)
{
    IntConverters instr{};
    switch (opcode)
    {
        case Instr::I32_WRAP_I64:
        {
            instr.kind = IntConverters::Wrap;
            break;
        }
        case Instr::I64_EXTEND_I32_S:
        {
            instr.kind = IntConverters::ExtendS;
            break;
        }
        case Instr::I64_EXTEND_I32_U:
        {
            instr.kind = IntConverters::ExtendU;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<IntConverters>, instr};
}

Instruction parse_FloatConverters(Instr opcode)
{
    FloatConverters instr{};
    switch (opcode)
    {
        case Instr::F32_DEMOTE_F64:
        {
            instr.kind = FloatConverters::Demote;
            break;
        }
        case Instr::F64_PROMOTE_F32:
        {
            instr.kind = FloatConverters::Promote;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<FloatConverters>, instr};
}

Instruction parse_FloatToIntTrunc(Instr opcode)
{
    FloatToIntTrunc instr{};
    switch (opcode)
    {
        case Instr::I32_TRUNC_F32_S:
        {
            instr.to = IntType::i32;
            instr.from = FloatType::f32;
            instr.is_signed = true;
            break;
        }
        case Instr::I32_TRUNC_F64_S:
        {
            instr.to = IntType::i32;
            instr.from = FloatType::f64;
            instr.is_signed = true;
            break;
        }
        case Instr::I64_TRUNC_F32_S:
        {
            instr.to = IntType::i64;
            instr.from = FloatType::f32;
            instr.is_signed = true;
            break;
        }
        case Instr::I64_TRUNC_F64_S:
        {
            instr.to = IntType::i64;
            instr.from = FloatType::f64;
            instr.is_signed = true;
            break;
        }
        case Instr::I32_TRUNC_F32_U:
        {
            instr.to = IntType::i32;
            instr.from = FloatType::f32;
            instr.is_signed = false;
            break;
        }
        case Instr::I32_TRUNC_F64_U:
        {
            instr.to = IntType::i32;
            instr.from = FloatType::f64;
            instr.is_signed = false;
            break;
        }
        case Instr::I64_TRUNC_F32_U:
        {
            instr.to = IntType::i64;
            instr.from = FloatType::f32;
            instr.is_signed = false;
            break;
        }
        case Instr::I64_TRUNC_F64_U:
        {
            instr.to = IntType::i64;
            instr.from = FloatType::f64;
            instr.is_signed = false;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<FloatToIntTrunc>, instr};
}

Instruction parse_IntToFloat(Instr opcode)
{
    IntToFloat instr{};
    switch (opcode)
    {
        case Instr::F32_CONVERT_I32_S:
        {
            instr.to = FloatType::f32;
            instr.from = IntType::i32;
            instr.is_signed = true;
            break;
        }
        case Instr::F32_CONVERT_I64_S:
        {
            instr.to = FloatType::f32;
            instr.from = IntType::i64;
            instr.is_signed = true;
            break;
        }
        case Instr::F64_CONVERT_I32_U:
        {
            instr.to = FloatType::f64;
            instr.from = IntType::i32;
            instr.is_signed = false;
            break;
        }
        case Instr::F64_CONVERT_I64_U:
        {
            instr.to = FloatType::f64;
            instr.from = IntType::i64;
            instr.is_signed = false;
            break;
        }
        case Instr::F32_CONVERT_I32_U:
        {
            instr.to = FloatType::f32;
            instr.from = IntType::i32;
            instr.is_signed = false;
            break;
        }
        case Instr::F32_CONVERT_I64_U:
        {
            instr.to = FloatType::f32;
            instr.from = IntType::i64;
            instr.is_signed = false;
            break;
        }
        case Instr::F64_CONVERT_I32_S:
        {
            instr.to = FloatType::f64;
            instr.from = IntType::i32;
            instr.is_signed = true;
            break;
        }
        case Instr::F64_CONVERT_I64_S:
        {
            instr.to = FloatType::f64;
            instr.from = IntType::i64;
            instr.is_signed = true;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<IntToFloat>, instr};
}

Instruction parse_ReinterpretBits(Instr opcode)
{
    ReinterpretBits instr{};
    switch (opcode)
    {
        case Instr::I32_REINTERPRET_F32:
        {
            instr.from = ValueType::f32;
            break;
        }
        case Instr::F32_REINTERPRET_I32:
        {
            instr.from = ValueType::i32;
            break;
        }
        case Instr::I64_REINTERPRET_F64:
        {
            instr.from = ValueType::f64;
            break;
        }
        case Instr::F64_REINTERPRET_I64:
        {
            instr.from = ValueType::i64;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<ReinterpretBits>, instr};
}

Instruction parse_Br(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode)
{
    Br instr{};
    size_t arg = leb128_decode(data, codeSize, offset);
    instr.index = arg;
    switch(opcode) 
    {
        case Instr::BR:
        {
            instr.is_unconditional = true;
            break;
        }
        case Instr::BR_IF:
        {
            instr.is_unconditional = false;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<Br>, instr};
}

Instruction parse_LoadConst(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode)
{
    LoadConst instr{};
    switch (opcode) 
    {
        case Instr::I32_CONST:
        {
		    int32_t arg = static_cast<int32_t>(leb128_decode(data, codeSize, offset));
            instr.value = arg;
            break;
        }
        case Instr::I64_CONST:
        {
		    int64_t arg = static_cast<int64_t>(leb128_decode(data, codeSize, offset));
            instr.value = arg;
            break;
        }
        case Instr::F32_CONST:
		{
			//Argument is IEEE7554 single point precision float stored in little endian format
			uint32_t bits = 0;
			for (int i = 0; i<=3; i++) {
				bits |= static_cast<uint32_t>(data[offset]) << 8*i;
				offset++;
			}
			float arg;
			std::memcpy(&arg, &bits, sizeof(float));
			instr.value = arg;
            break;
		}
        case Instr::F64_CONST:
		{
			//Argument is IEEE7554 single point precision float stored in little endian format
			uint64_t bits = 0;
			for (int i = 0; i<=7; i++) {
				bits |= static_cast<uint32_t>(data[offset]) << 8*i;
				offset++;
			}
			double arg;
			std::memcpy(&arg, &bits, sizeof(double));
			instr.value = arg;
            break;
		}
        default: //this should never happen so blindly break
        {  
            break;
        }
    }

    return Instruction{std::in_place_type<LoadConst>, instr};
}

Instruction parse_Scope(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode, size_t instrvectorEnd)
{   
    Scope instr{};
    instr.info.block_start = instrvectorEnd;
    instr.else_info = std::nullopt;

    switch (opcode)
    {
        case Instr::BLOCK:
        {
            instr.kind = Scope::Kind::Block;
            break;
        }
        case Instr::LOOP:
        {
            instr.kind = Scope::Kind::Loop;
            break;
        }
        case Instr::IF:
        {
            instr.kind = Scope::Kind::If;
            break;
        }
        default: //this should never happen so blindly break
        {  
            break;
        }
    }

    //Reading return type of block, loop, or if block
    if (data[offset] != 0x40) //0x40 means that the block returns nothing 
    {
        ValueType type = static_cast<ValueType>(data[offset]);
        instr.info.return_type = type;
    }
    else
    {
        instr.info.return_type = std::nullopt;
    }
    

    ++offset; //move past the return type

    return Instruction{std::in_place_type<Scope>, instr};
}

Instruction parse_Local(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode)
{
    Local instr{};
    size_t arg = leb128_decode(data, codeSize, offset);
    instr.index = arg;
    switch (opcode)
    {
        case Instr::LOCAL_GET:
        {
            instr.kind = Local::Kind::Get;
            break;
        }
        case Instr::LOCAL_SET:
        {
            instr.kind = Local::Kind::Set;
            break;
        }
        case Instr::LOCAL_TEE:
        {
            instr.kind = Local::Kind::Tee;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<Local>, instr};
}

Instruction parse_Global(std::span<const uint8_t> data, size_t codeSize, size_t&  offset, Instr opcode)
{
    Global instr{};
    size_t arg = leb128_decode(data, codeSize, offset);
    instr.index = arg;

    switch (opcode)
    {
        case Instr::GLOBAL_GET:
        {
            instr.kind = Global::Kind::Get;
            break;
        }
        case Instr::GLOBAL_SET:
        {
            instr.kind = Global::Kind::Set;
            break;
        }
        default: //this should never happen so blindly break
        {
            break;
        }
    }

    return Instruction{std::in_place_type<Global>, instr};
}

