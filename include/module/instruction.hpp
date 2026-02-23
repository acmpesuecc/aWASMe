#pragma once
#include<vector>
#include<string>
#include<variant>
#include<cstdint>
#include<stdfloat>
#include<optional>

//Type definitions
using Value = std::variant<int32_t, int64_t, float, double>;

enum class InstrKind 
{
	//Control flow related
	UNREACABLE, NOP, 
	BLOCK, LOOP, IF, ELSE, END, BR, BR_IF, BR_TABLE, RETURN,
	CALL, CALL_INDIRECT,

	//Fetching data from stack
	DROP, SELECT, 
	LOCAL_GET, LOCAL_SET, LOCAL_TEE,
	GLOBAL_GET, GLOBAL_SET, 
	I32_LOAD, I64_LOAD, F32_LOAD, F64_LOAD, I32_LOAD8_S, I32_LOAD8_U, I32_LOAD16_S, I32_LOAD16_U, 
		I64_LOAD8_S, I64_LOAD8_U, I64_LOAD16_S, I64_LOAD16_U, I64_LOAD32_S, I64_LOAD32_U,

	//Writing into memory?
	I32_STORE, I64_STORE, F32_STORE, F64_STORE, I32_STORE8, I32_STORE16, I64_STORE8, I64_STORE16, I64_STORE32,
	MEMORY_GROW,

	//Pushing values onto stack
	I32_CONST, I64_CONST, F32_CONST, F64_CONST,

	//Comparison operations
	I32_EQZ, I32_EQ, I32_NE, I32_LT_S, I32_LT_U, I32_GT_S, I32_GT_U, I32_LE_S, I32_LE_U, I32_GE_S, I32_GE_U,
	I64_EQZ, I64_EQ, I64_NE, I64_LT_S, I64_LT_U, I64_GT_S, I64_GT_U, I64_LE_S, I64_LE_U, I64_GE_S, I64_GE_U,
	F32_EQ, F32_NE, F32_LT, F32_GT, F32_LE, F32_GE, 
	F64_EQ, F64_NE, F64_LT, F64_GT, F64_LE, F64_GE, 

	//Arithmetic operations
	I32_CLZ, I32_CTZ, I32_POPCNT, I32_ADD, I32_SUB, I32_MUL, I32_DIV, I32_DIV_S, I32_DIV_U, I23_REM_S, I32_REM_U, 
		I32_AND, I32_OR, I32_XOR, I32_SHL, I32_SHR_S, I32_SHR_U, I32_ROTL, I32_ROTR, 
	I64_CLZ, I64_CTZ, I64_POPCNT, I64_ADD, I64_SUB, I64_MUL, I64_DIV, I64_DIV_S, I64_DIV_U, I64_REM_S, I64_REM_U, 
		I64_AND, I64_OR, I64_XOR, I64_SHL, I64_SHR_S, I64_SHR_U, I64_ROTL, I64_ROTR,
	F32_ABS, F32_NEG, F32_CEIL, F32_FLOOR, F32_TRUNC, F32_NEAREST, F32_SQRT, F32_ADD, F32_SUB, F32_MUL, F32_DIV, F32_MIN, F32_MAX, F32_COPYSIGN,
	F64_ABS, F64_NEG, F64_CEIL, F64_FLOOR, F64_TRUNC, F64_NEAREST, F64_SQRT, F64_ADD, F64_SUB, F64_MUL, F64_DIV, F64_MIN, F64_MAX, F64_COPYSIGN,

	//Data type conversions
	I32_WRAP_I64, I32_TRUNC_F32_S, I32_TRUNC_F32_U, I32_TRUNC_F64_S, I32_TRUNC_F64_U, 
	I64_EXTEND_I32_S, I64_EXTEND_I32_U, I64_TRUNC_F32_S, I64_TRUNC_F32_U, I64_TRUNC_F64_S, I64_TRUNC_F64_U, 
	F32_CONVERT_I32_S, F32_CONVERT_I32_U, F32_CONVERT_I64_S, F32_CONVERT_I64_U, 
	F32_DEMOTE_F64, F64_CONVERT_I32_S, F64_CONVERT_I32_U, F64_CONVERT_I64_S, F64_CONVERT_I64_U,
	F64_PROMOTE_F32, I32_REINTERPRET_F32, I64_REINTERPRET_F64, F32_REINTERPRET_I32, F64_REINTERPRET_I64
};

struct BlockInfo 
{
	size_t block_start; // index into the start of the block (i.e, the instruction which creates the block and NOT the first instruction of the block)
	size_t block_end; // index into the `end` instruction of the block 
};

union InstrArgs {
	Value value;	

	// For `loop`s, `block`s, `if`s, 'else's
	BlockInfo block_kind;

	struct {
		BlockInfo if_block_info;
		std::optional<BlockInfo> else_block_info;
	} if_;
};

struct Instruction 
{
    InstrKind kind;

    std::optional<InstrArgs> args;

    Instruction(InstrKind k):
        kind(k), args(std::nullopt) {}

    Instruction(InstrKind k, InstrArgs a):
        kind(k), args(a) {}
};