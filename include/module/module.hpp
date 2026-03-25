#pragma once
#include<vector>
#include<cstdint>
#include<optional>
#include "instruction.hpp"

class Module 
{
	public:

	//STRUCT DEFINITIONS
	struct LocalData
	{ 
		int i32; //only storing count here because the locals are initialized in the function code which the engine has to run
		int i64;
		int f32;
		int f64;  
		
		LocalData() 
		{
			i32 = 0;
			i64 = 0;
			f32 = 0;
			f64 = 0;
		}
	};

	struct Function
	{
		uint8_t typeIndex;
		struct LocalData localCounts;
		std::vector<Instruction> code; 

		Function()
		{
			//typeIndex will be initialized during function section parsing
			localCounts = LocalData();
			code = std::vector<Instruction>{};
		}
	};

	struct Global
	{
		ValueType type;
		bool is_mutable;
		std::vector<Instruction> initCode;
	};
	
	struct Type //currently only supports function signatures
	{
		std::vector<ValueType> params;
		//currently WASM 1.0 only allows 1 return type max per function, but this may increase later so parser accepts multiple
		std::vector<ValueType> returns; 
	};

	struct Memory
	{
		IntType address_type;
		size_t start_page;
		std::optional<size_t> end_page;
	};

	//MODULE STRUCTURE
	std::vector<Function> functions;      
	std::optional<uint32_t> start_function;	
	std::vector<Global> globals;	
	std::vector<Type> types;
	std::vector<Memory> memories; //currently WASM 1.0 allows only one 1 memory max, but this may increase later so parser accepts multiple        

	/* 
	Will implement these in the future
		std::vector<Export> exports;          
	*/

	//Constructor
	Module()
	{
		functions = std::vector<Function>{};
		start_function = std::nullopt;
		globals = std::vector<Global>{};
		types = std::vector<Type>{};
		memories = std::vector<Memory>{};
	}
};