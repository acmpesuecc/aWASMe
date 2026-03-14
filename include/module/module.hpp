#pragma once
#include<vector>
#include<cstdint>
#include<optional>
#include "instruction.hpp"

//Local variable data for function data structure
struct LocalData
{ 
	int i32_count; //only storing count here because the locals are initialized in the function code which the engine has to run
	int i64_count;
	int f32_count;
	int f64_count;  
	
	LocalData() 
	{
		i32_count = 0;
		i64_count = 0;
		f32_count = 0;
		f64_count = 0;
	}
};

//Function data structure
struct Function
{
    uint8_t typeIndex;
	struct LocalData localData;
    std::vector<Instruction> funcCode; 

	Function()
	{
		//we will read the function section to get the type index and initialize the typeIndex value then
		localData = LocalData();
		funcCode = std::vector<Instruction>();
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
			//not sure how to assign start_function - what value should it take when the start_function is undefined?
		}
};