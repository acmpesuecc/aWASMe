#pragma once
#include<cstdint>
#include<span>
#include<unordered_map>
#include<iostream>
#include "module/module.hpp"

//Binary to opcode struct
enum class Instr : uint8_t
{
	//Control flow related
	UNREACHABLE = 0x0, NOP = 0x1, 
	BLOCK = 0x2, LOOP = 0x3, IF = 0x4, ELSE = 0x5, END = 0xb, 
	BR = 0xc, BR_IF = 0xd, BR_TABLE = 0xe, RETURN = 0xf, 
	CALL = 0x10, CALL_INDIRECT = 0x11, 
	
	//Fetching data from stack
	DROP = 0x1a, SELECT = 0x1b, 
	LOCAL_GET = 0x20, LOCAL_SET = 0x21, LOCAL_TEE = 0x22, 
	GLOBAL_GET = 0x23, GLOBAL_SET = 0x24, 
	I32_LOAD = 0x28, I64_LOAD = 0x29, F32_LOAD = 0x2a, F64_LOAD = 0x2b, I32_LOAD8_S = 0x2c, 
	I32_LOAD8_U = 0x2d, I32_LOAD16_S = 0x2e, I32_LOAD16_U = 0x2f, I64_LOAD8_S = 0x30, 
	I64_LOAD8_U = 0x31, I64_LOAD16_S = 0x32, I64_LOAD16_U = 0x33, I64_LOAD32_S = 0x34, I64_LOAD32_U = 0x35, 
	
	//Writing into memory?
	I32_STORE = 0x36, I64_STORE = 0x37, F32_STORE = 0x38, F64_STORE = 0x39, I32_STORE8 = 0x3a, 
	I32_STORE16 = 0x3b, I64_STORE8 = 0x3c, I64_STORE16 = 0x3d, I64_STORE32 = 0x3e, 
	MEMORY_SIZE = 0x3f, MEMORY_GROW = 0x40, 
	
	//Pushing values onto stack
	I32_CONST = 0x41, I64_CONST = 0x42, F32_CONST = 0x43, F64_CONST = 0x44, 
	
	//Comparison operations
	I32_EQZ = 0x45, I32_EQ = 0x46, I32_NE = 0x47, I32_LT_S = 0x48, I32_LT_U = 0x49, I32_GT_S = 0x4a, 
	I32_GT_U = 0x4b, I32_LE_S = 0x4c, I32_LE_U = 0x4d, I32_GE_S = 0x4e, I32_GE_U = 0x4f, I64_EQZ = 0x50, 
	I64_EQ = 0x51, I64_NE = 0x52, I64_LT_S = 0x53, I64_LT_U = 0x54, I64_GT_S = 0x55, I64_GT_U = 0x56, 
	I64_LE_S = 0x57, I64_LE_U = 0x58, I64_GE_S = 0x59, I64_GE_U = 0x5a, F32_EQ = 0x5b, F32_NE = 0x5c, 
	F32_LT = 0x5d, F32_GT = 0x5e, F32_LE = 0x5f, F32_GE = 0x60, F64_EQ = 0x61, F64_NE = 0x62, F64_LT = 0x63, 
	F64_GT = 0x64, F64_LE = 0x65, F64_GE = 0x66, 
	
	//Arithmetic operations
	I32_CLZ = 0x67, I32_CTZ = 0x68, I32_POPCNT = 0x69, I32_ADD = 0x6a, I32_SUB = 0x6b, I32_MUL = 0x6c, 
	I32_DIV_S = 0x6d, I32_DIV_U = 0x6e, I32_REM_S = 0x6f, I32_REM_U = 0x70, I32_AND = 0x71, I32_OR = 0x72, 
	I32_XOR = 0x73, I32_SHL = 0x74, I32_SHR_S = 0x75, I32_SHR_U = 0x76, I32_ROTL = 0x77, I32_ROTR = 0x78, 
	I64_CLZ = 0x79, I64_CTZ = 0x7a, I64_POPCNT = 0x7b, I64_ADD = 0x7c, I64_SUB = 0x7d, I64_MUL = 0x7e, 
	I64_DIV_S = 0x7f, I64_DIV_U = 0x80, I64_REM_S = 0x81, I64_REM_U = 0x82, I64_AND = 0x83, I64_OR = 0x84, 
	I64_XOR = 0x85, I64_SHL = 0x86, I64_SHR_S = 0x87, I64_SHR_U = 0x88, I64_ROTL = 0x89, I64_ROTR = 0x8a, 
	F32_ABS = 0x8b, F32_NEG = 0x8c, F32_CEIL = 0x8d, F32_FLOOR = 0x8e, F32_TRUNC = 0x8f, F32_NEAREST = 0x90, 
	F32_SQRT = 0x91, F32_ADD = 0x92, F32_SUB = 0x93, F32_MUL = 0x94, F32_DIV = 0x95, F32_MIN = 0x96, 
	F32_MAX = 0x97, F32_COPYSIGN = 0x98, F64_ABS = 0x99, F64_NEG = 0x9a, F64_CEIL = 0x9b, F64_FLOOR = 0x9c, 
	F64_TRUNC = 0x9d, F64_NEAREST = 0x9e, F64_SQRT = 0x9f, F64_ADD = 0xa0, F64_SUB = 0xa1, F64_MUL = 0xa2, 
	F64_DIV = 0xa3, F64_MIN = 0xa4, F64_MAX = 0xa5, F64_COPYSIGN = 0xa6, 
	
	//Data type conversions
	I32_WRAP_I64 = 0xa7, I32_TRUNC_F32_S = 0xa8, I32_TRUNC_F32_U = 0xa9, I32_TRUNC_F64_S = 0xaa, 
	I32_TRUNC_F64_U = 0xab, I64_EXTEND_I32_S = 0xac, I64_EXTEND_I32_U = 0xad, I64_TRUNC_F32_S = 0xae, 
	I64_TRUNC_F32_U = 0xaf, I64_TRUNC_F64_S = 0xb0, I64_TRUNC_F64_U = 0xb1, F32_CONVERT_I32_S = 0xb2, 
	F32_CONVERT_I32_U = 0xb3, F32_CONVERT_I64_S = 0xb4, F32_CONVERT_I64_U = 0xb5, F32_DEMOTE_F64 = 0xb6, 
	F64_CONVERT_I32_S = 0xb7, F64_CONVERT_I32_U = 0xb8, F64_CONVERT_I64_S = 0xb9, F64_CONVERT_I64_U = 0xba, 
	F64_PROMOTE_F32 = 0xbb, I32_REINTERPRET_F32 = 0xbc, I64_REINTERPRET_F64 = 0xbd, F32_REINTERPRET_I32 = 0xbe, 
	F64_REINTERPRET_I64 = 0xbf 
};


enum class InstrCategory
{
	//EMPTY STRUCTS
	Nop,
	Unreachable,
	End,
	Return,

	//STRUCTS WITH CONTENTS
	//Control flow
	Scope,
	Call,
	Br,

	//Calculation related
	IntArithmetic,
	FloatArithmetic,
	UnaryInt,
	UnaryFloat,
	BinaryFloat,
	UnaryBitwise, //not sure if we require this one rn bc I32_NOT and I64_NOT don't exist
	BinaryBitwise, //only integers can do the operations in this category and the category above this one
	IntCmp,
	FloatCmp,
	IntConverters,
	FloatConverters,
	FloatToIntTrunc,
	IntToFloat,
	ReinterpretBits,

	//Defined by the parser
	UNKNOWN,
	ELSE,

	//Other
	Local,
	Global,
	LoadConst,
};

//Categorizing instructions
inline std::unordered_map<Instr, InstrCategory> Instr_to_Category = 
{
	{Instr::UNREACHABLE, InstrCategory::Unreachable},					{Instr::NOP, InstrCategory::Nop},			
	{Instr::BLOCK, InstrCategory::Scope},								{Instr::LOOP, InstrCategory::Scope},			
	{Instr::IF, InstrCategory::Scope},									{Instr::ELSE, InstrCategory::ELSE},			
	{Instr::END, InstrCategory::End},									{Instr::BR, InstrCategory::Br},			
	{Instr::BR_IF, InstrCategory::Br},									{Instr::BR_TABLE, InstrCategory::UNKNOWN},			
	{Instr::RETURN, InstrCategory::Return},								{Instr::CALL, InstrCategory::Call},			
	{Instr::CALL_INDIRECT, InstrCategory::UNKNOWN},						{Instr::DROP, InstrCategory::UNKNOWN},			
	{Instr::SELECT, InstrCategory::UNKNOWN},							{Instr::LOCAL_GET, InstrCategory::Local},			
	{Instr::LOCAL_SET, InstrCategory::Local},							{Instr::LOCAL_TEE, InstrCategory::Local},			
	{Instr::GLOBAL_GET, InstrCategory::Global},							{Instr::GLOBAL_SET, InstrCategory::Global},			
	{Instr::I32_LOAD, InstrCategory::UNKNOWN},							{Instr::I64_LOAD, InstrCategory::UNKNOWN},			
	{Instr::F32_LOAD, InstrCategory::UNKNOWN},							{Instr::F64_LOAD, InstrCategory::UNKNOWN},			
	{Instr::I32_LOAD8_S, InstrCategory::UNKNOWN},						{Instr::I32_LOAD8_U, InstrCategory::UNKNOWN},			
	{Instr::I32_LOAD16_S, InstrCategory::UNKNOWN},						{Instr::I32_LOAD16_U, InstrCategory::UNKNOWN},			
	{Instr::I64_LOAD8_S, InstrCategory::UNKNOWN},						{Instr::I64_LOAD8_U, InstrCategory::UNKNOWN},			
	{Instr::I64_LOAD16_S, InstrCategory::UNKNOWN},						{Instr::I64_LOAD16_U, InstrCategory::UNKNOWN},			
	{Instr::I64_LOAD32_S, InstrCategory::UNKNOWN},						{Instr::I64_LOAD32_U, InstrCategory::UNKNOWN},			
	{Instr::I32_STORE, InstrCategory::UNKNOWN},							{Instr::I64_STORE, InstrCategory::UNKNOWN},			
	{Instr::F32_STORE, InstrCategory::UNKNOWN},							{Instr::F64_STORE, InstrCategory::UNKNOWN},			
	{Instr::I32_STORE8, InstrCategory::UNKNOWN},						{Instr::I32_STORE16, InstrCategory::UNKNOWN},			
	{Instr::I64_STORE8, InstrCategory::UNKNOWN},						{Instr::I64_STORE16, InstrCategory::UNKNOWN},			
	{Instr::I64_STORE32, InstrCategory::UNKNOWN},						{Instr::MEMORY_SIZE, InstrCategory::UNKNOWN},			
	{Instr::MEMORY_GROW, InstrCategory::UNKNOWN},						{Instr::I32_CONST, InstrCategory::LoadConst},			
	{Instr::I64_CONST, InstrCategory::LoadConst},						{Instr::F32_CONST, InstrCategory::LoadConst},			
	{Instr::F64_CONST, InstrCategory::LoadConst},						{Instr::I32_EQZ, InstrCategory::UNKNOWN},			
	{Instr::I32_EQ, InstrCategory::IntCmp},								{Instr::I32_NE, InstrCategory::IntCmp},			
	{Instr::I32_LT_S, InstrCategory::IntCmp},							{Instr::I32_LT_U, InstrCategory::IntCmp},			
	{Instr::I32_GT_S, InstrCategory::IntCmp},							{Instr::I32_GT_U, InstrCategory::IntCmp},			
	{Instr::I32_LE_S, InstrCategory::IntCmp},							{Instr::I32_LE_U, InstrCategory::IntCmp},			
	{Instr::I32_GE_S, InstrCategory::IntCmp},							{Instr::I32_GE_U, InstrCategory::IntCmp},			
	{Instr::I64_EQZ, InstrCategory::UNKNOWN},							{Instr::I64_EQ, InstrCategory::IntCmp},			
	{Instr::I64_NE, InstrCategory::IntCmp},								{Instr::I64_LT_S, InstrCategory::IntCmp},			
	{Instr::I64_LT_U, InstrCategory::IntCmp},							{Instr::I64_GT_S, InstrCategory::IntCmp},			
	{Instr::I64_GT_U, InstrCategory::IntCmp},							{Instr::I64_LE_S, InstrCategory::IntCmp},			
	{Instr::I64_LE_U, InstrCategory::IntCmp},							{Instr::I64_GE_S, InstrCategory::IntCmp},			
	{Instr::I64_GE_U, InstrCategory::IntCmp},							{Instr::F32_EQ, InstrCategory::FloatCmp},			
	{Instr::F32_NE, InstrCategory::FloatCmp},							{Instr::F32_LT, InstrCategory::FloatCmp},			
	{Instr::F32_GT, InstrCategory::FloatCmp},							{Instr::F32_LE, InstrCategory::FloatCmp},			
	{Instr::F32_GE, InstrCategory::FloatCmp},							{Instr::F64_EQ, InstrCategory::FloatCmp},			
	{Instr::F64_NE, InstrCategory::FloatCmp},							{Instr::F64_LT, InstrCategory::FloatCmp},			
	{Instr::F64_GT, InstrCategory::FloatCmp},							{Instr::F64_LE, InstrCategory::FloatCmp},			
	{Instr::F64_GE, InstrCategory::FloatCmp},							{Instr::I32_CLZ, InstrCategory::UnaryInt},			
	{Instr::I32_CTZ, InstrCategory::UnaryInt},							{Instr::I32_POPCNT, InstrCategory::UnaryInt},			
	{Instr::I32_ADD, InstrCategory::IntArithmetic},						{Instr::I32_SUB, InstrCategory::IntArithmetic},			
	{Instr::I32_MUL, InstrCategory::IntArithmetic},						{Instr::I32_DIV_S, InstrCategory::IntArithmetic},			
	{Instr::I32_DIV_U, InstrCategory::IntArithmetic},					{Instr::I32_REM_S, InstrCategory::IntArithmetic},			
	{Instr::I32_REM_U, InstrCategory::IntArithmetic},					{Instr::I32_AND, InstrCategory::BinaryBitwise},			
	{Instr::I32_OR, InstrCategory::BinaryBitwise},						{Instr::I32_XOR, InstrCategory::BinaryBitwise},			
	{Instr::I32_SHL, InstrCategory::BinaryBitwise},						{Instr::I32_SHR_S, InstrCategory::BinaryBitwise},			
	{Instr::I32_SHR_U, InstrCategory::BinaryBitwise},					{Instr::I32_ROTL, InstrCategory::UNKNOWN},			
	{Instr::I32_ROTR, InstrCategory::UNKNOWN},							{Instr::I64_CLZ, InstrCategory::UnaryInt},			
	{Instr::I64_CTZ, InstrCategory::UnaryInt},							{Instr::I64_POPCNT, InstrCategory::UnaryInt},			
	{Instr::I64_ADD, InstrCategory::IntArithmetic},						{Instr::I64_SUB, InstrCategory::IntArithmetic},			
	{Instr::I64_MUL, InstrCategory::IntArithmetic},						{Instr::I64_DIV_S, InstrCategory::IntArithmetic},			
	{Instr::I64_DIV_U, InstrCategory::IntArithmetic},					{Instr::I64_REM_S, InstrCategory::IntArithmetic},			
	{Instr::I64_REM_U, InstrCategory::IntArithmetic},					{Instr::I64_AND, InstrCategory::BinaryBitwise},			
	{Instr::I64_OR, InstrCategory::BinaryBitwise},						{Instr::I64_XOR, InstrCategory::BinaryBitwise},			
	{Instr::I64_SHL, InstrCategory::BinaryBitwise},						{Instr::I64_SHR_S, InstrCategory::BinaryBitwise},			
	{Instr::I64_SHR_U, InstrCategory::BinaryBitwise},					{Instr::I64_ROTL, InstrCategory::UNKNOWN},			
	{Instr::I64_ROTR, InstrCategory::UNKNOWN},							{Instr::F32_ABS, InstrCategory::UnaryFloat},			
	{Instr::F32_NEG, InstrCategory::UnaryFloat},						{Instr::F32_CEIL, InstrCategory::UnaryFloat},			
	{Instr::F32_FLOOR, InstrCategory::UnaryFloat},						{Instr::F32_TRUNC, InstrCategory::UnaryFloat},			
	{Instr::F32_NEAREST, InstrCategory::UnaryFloat},					{Instr::F32_SQRT, InstrCategory::UnaryFloat},			
	{Instr::F32_ADD, InstrCategory::FloatArithmetic},					{Instr::F32_SUB, InstrCategory::FloatArithmetic},			
	{Instr::F32_MUL, InstrCategory::FloatArithmetic},					{Instr::F32_DIV, InstrCategory::FloatArithmetic},			
	{Instr::F32_MIN, InstrCategory::BinaryFloat},						{Instr::F32_MAX, InstrCategory::BinaryFloat},			
	{Instr::F32_COPYSIGN, InstrCategory::BinaryFloat},					{Instr::F64_ABS, InstrCategory::UnaryFloat},			
	{Instr::F64_NEG, InstrCategory::UnaryFloat},						{Instr::F64_CEIL, InstrCategory::UnaryFloat},			
	{Instr::F64_FLOOR, InstrCategory::UnaryFloat},						{Instr::F64_TRUNC, InstrCategory::UnaryFloat},			
	{Instr::F64_NEAREST, InstrCategory::UnaryFloat},					{Instr::F64_SQRT, InstrCategory::UnaryFloat},			
	{Instr::F64_ADD, InstrCategory::FloatArithmetic},					{Instr::F64_SUB, InstrCategory::FloatArithmetic},			
	{Instr::F64_MUL, InstrCategory::FloatArithmetic},					{Instr::F64_DIV, InstrCategory::FloatArithmetic},			
	{Instr::F64_MIN, InstrCategory::BinaryFloat},						{Instr::F64_MAX, InstrCategory::BinaryFloat},			
	{Instr::F64_COPYSIGN, InstrCategory::BinaryFloat},					{Instr::I32_WRAP_I64, InstrCategory::IntConverters},			
	{Instr::I32_TRUNC_F32_S, InstrCategory::FloatToIntTrunc},			{Instr::I32_TRUNC_F32_U, InstrCategory::FloatToIntTrunc},			
	{Instr::I32_TRUNC_F64_S, InstrCategory::FloatToIntTrunc},			{Instr::I32_TRUNC_F64_U, InstrCategory::FloatToIntTrunc},			
	{Instr::I64_EXTEND_I32_S, InstrCategory::IntConverters},			{Instr::I64_EXTEND_I32_U, InstrCategory::IntConverters},			
	{Instr::I64_TRUNC_F32_S, InstrCategory::FloatToIntTrunc},			{Instr::I64_TRUNC_F32_U, InstrCategory::FloatToIntTrunc},			
	{Instr::I64_TRUNC_F64_S, InstrCategory::FloatToIntTrunc},			{Instr::I64_TRUNC_F64_U, InstrCategory::FloatToIntTrunc},			
	{Instr::F32_CONVERT_I32_S, InstrCategory::IntToFloat},				{Instr::F32_CONVERT_I32_U, InstrCategory::IntToFloat},			
	{Instr::F32_CONVERT_I64_S, InstrCategory::IntToFloat},				{Instr::F32_CONVERT_I64_U, InstrCategory::IntToFloat},			
	{Instr::F32_DEMOTE_F64, InstrCategory::FloatConverters},			{Instr::F64_CONVERT_I32_S, InstrCategory::IntToFloat},			
	{Instr::F64_CONVERT_I32_U, InstrCategory::IntToFloat},				{Instr::F64_CONVERT_I64_S, InstrCategory::IntToFloat},			
	{Instr::F64_CONVERT_I64_U, InstrCategory::IntToFloat},				{Instr::F64_PROMOTE_F32, InstrCategory::FloatConverters},			
	{Instr::I32_REINTERPRET_F32, InstrCategory::ReinterpretBits},		{Instr::I64_REINTERPRET_F64, InstrCategory::ReinterpretBits},			
	{Instr::F32_REINTERPRET_I32, InstrCategory::ReinterpretBits},		{Instr::F64_REINTERPRET_I64, InstrCategory::ReinterpretBits}	
};

//Parsing functions
std::vector<Instruction> parse_code(std::span<const uint8_t> data, size_t& offset);
Instruction parse_IntArithmetic(Instr opcode);
Instruction parse_FloatArithmetic(Instr opcode);
Instruction parse_UnaryInt(Instr opcode);
Instruction parse_UnaryFloat(Instr opcode);
Instruction parse_BinaryFloat(Instr opcode);
Instruction parse_BinaryBitwise(Instr opcode); //Only integers can perform these operations
Instruction parse_IntCmp(Instr opcode); //Only integers can perform these operations
Instruction parse_FloatCmp(Instr opcode);
Instruction parse_IntConverters(Instr opcode);
Instruction parse_FloatConverters(Instr opcode);
Instruction parse_FloatToIntTrunc(Instr opcode);
Instruction parse_IntToFloat(Instr opcode);
Instruction parse_ReinterpretBits(Instr opcode);
Instruction parse_LoadConst(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode);
Instruction parse_Scope(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode, size_t instrvectorEnd);
Instruction parse_Local(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode);
Instruction parse_Global(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode);
Instruction parse_Br(std::span<const uint8_t> data, size_t codeSize, size_t& offset, Instr opcode);

//Helper to read leb128 values (defined in parser.cpp)
size_t leb128_decode(std::span<const uint8_t> data, size_t size, size_t& offset);

//Debugging helper to print opcodes onto the screen (defined in parser.cpp)
void print_string_of_opcode(Instr opcode);
