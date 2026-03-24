#pragma once
#include<vector>
#include<cstdint>
#include<optional>
#include "instruction.hpp"

//Local variable data for function data structure
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

//Function data structure
struct Function
{
    uint8_t typeIndex;
	struct LocalData localCounts;
    std::vector<Instruction> code; 

	Function()
	{
		//we will read the function section to get the type index and initialize the typeIndex value then
		localCounts = LocalData();
		code = std::vector<Instruction>();
	}
};


//Module = data being sent to the engine
class Module 
{
    public:
		std::size_t filesize;
		std::vector<Function> functions;      
		std::optional<uint32_t> start_function;		
		/* 
		Will implement these in the future
			std::vector<Memory> memories;         
			std::vector<Export> exports;          
			std::vector<Global> globals;
		*/

		//Constructors
		Module(int s)
		{
			filesize = s;
			functions = std::vector<Function>();
			start_function = std::nullopt;
		}
};