#pragma once
#include<vector>
#include<cstdint>
#include<optional>
#include "instruction.hpp"

//Function data structure
struct Function
{
    uint8_t funcIndex;
    std::vector<Instruction> funcInstrs; 
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