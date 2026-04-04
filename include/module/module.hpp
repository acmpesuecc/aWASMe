#pragma once
#include<vector>
#include<cstdint>
#include<optional>
#include<variant>
#include "instruction.hpp"

enum class ExternalType : uint8_t //used in imports and exports
{
       Function     = 0x00,
       Table        = 0x01,
       Memory       = 0x02,
       Global       = 0x03, 
       Tag          = 0x04,
       //Note: not supporting tag types or tables currently
};

class Module 
{
	public:

	//Interface struct definitions for imports - currently only supports globals, memories and functions
	struct GlobalInterface
	{
		ValueType type;
		bool is_mutable;	
	};

	struct MemoryInterface
	{
		IntType address_type;
		size_t start_page;
		std::optional<size_t> end_page;
	};

	struct FunctionInterface
	{
		size_t typeIndex;
	};
	typedef std::variant<MemoryInterface, GlobalInterface, FunctionInterface> ImportInterface;


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
		size_t typeIndex;
		struct LocalData localCounts;
		std::vector<Instruction> code;
		std::vector<ValueType> locals;
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
		std::vector<ValueType> returns; 
			//currently WASM 1.0 only allows 1 return type max per function, but this may increase later so parser accepts multiple
	};

	struct Import
	{
		std::string module_name;
		std::string item_name;
		ExternalType type;
		ImportInterface interface;
	};

	struct Export
	{
		std::string name;
		ExternalType type;
		size_t index;
	};

	//MODULE STRUCTURE
	std::vector<Function> functions;      
	std::optional<size_t> start_function;	
	std::vector<Global> globals;	
	std::vector<Type> types;
	std::vector<MemoryInterface> memories; 
		//parser has no extra information to store in memories besides what is already in the memory interface, so it is reused here
		//currently WASM 1.0 allows only one 1 memory max, but this may increase later so parser accepts multiple        
	std::vector<Import> imports;       
	std::vector<Export> exports;   

	//Constructor
	Module()
	{
		functions = std::vector<Function>{};
		start_function = std::nullopt;
		globals = std::vector<Global>{};
		types = std::vector<Type>{};
		memories = std::vector<MemoryInterface>{};  
		imports = std::vector<Import>{};
		exports = std::vector<Export>{};
	}
};