#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<cstring> //for std::memcpy()
#include<span>
#include "module/module.hpp"
#include "parser/parser.hpp" 

//Section parsing
void parse_code_section(std::span<const uint8_t> data, Module& module) {

	size_t secSize = data.size();
	size_t offset = 0;
	size_t num_funcs = leb128_decode(data, secSize, offset);
	std::cout << "Number of functions defined: " << num_funcs << std::endl;
	
	for (int i = 0; i < num_funcs; i++) 
	{
		size_t bodySize = leb128_decode(data, secSize, offset);
		size_t endOfBody = offset + bodySize;
		std::cout << "Function " << i << " body size: " << bodySize << std::endl;


		size_t numlocals = leb128_decode(data, secSize, offset);
		std::cout << "Number of locals defined: " << numlocals << std::endl;

		while (numlocals)
		{
			size_t numlocals_specifictype = leb128_decode(data, secSize, offset);
			numlocals -= numlocals_specifictype;
			Type type = static_cast<Type>(data[offset]);
			switch (type) {
				case Type::I32: 
					std::cout << numlocals_specifictype << " locals defined of type I32" << std::endl;
					break;
				case Type::I64: 
					std::cout << numlocals_specifictype << " locals defined of type I64" << std::endl;
					break;
				case Type::F32: 
					std::cout << numlocals_specifictype << " locals defined of type F32" << std::endl;
					break;
				case Type::F64: 
					std::cout << numlocals_specifictype << " locals defined of type F64" << std::endl;
					break;
				case Type::I32_const: 
					std::cout << numlocals_specifictype << " locals defined of type I32_const" << std::endl;
					break;
				case Type::I64_const: 
					std::cout << numlocals_specifictype << " locals defined of type I64_const" << std::endl;
					break;
				case Type::F32_const: 
					std::cout << numlocals_specifictype << " locals defined of type F32_const" << std::endl;
					break;				
				case Type::F64_const: 
					std::cout << numlocals_specifictype << " locals defined of type F64_const" << std::endl;
					break;
				case Type::Global_get: 
					std::cout << numlocals_specifictype << " locals defined of type Global_get" << std::endl;
					break;
				default:
					std::cout << "Error - invalid type" << std::endl;		
					break;
			}
			offset++; //advance offset
		}

		std::cout << "Function body is as follows: " << std::endl;
		while (offset < endOfBody) 
		{
			InstrKind opcode = static_cast<InstrKind>(data[offset]);
			print_string_of_opcode(opcode);
			offset++; //advance offset
			switch (opcode) { //check if opcode takes any arguments, and if so read and display them
				case InstrKind::BR:
				case InstrKind::BR_IF:
				case InstrKind::LOCAL_GET:
				case InstrKind::LOCAL_SET:
				case InstrKind::GLOBAL_GET:
				case InstrKind::GLOBAL_SET:
				case InstrKind::MEMORY_SIZE:
				case InstrKind::MEMORY_GROW:
				case InstrKind::CALL:
				case InstrKind::I32_CONST:
				case InstrKind::I64_CONST:
				case InstrKind::BLOCK:
				case InstrKind::LOOP:
				case InstrKind::IF: 
				{
					size_t arg = leb128_decode(data, secSize, offset);
					std::cout << "\t1 argument with value: " << arg << std::endl;
					break;
				}
				case InstrKind::CALL_INDIRECT:
				{
					size_t arg1 = leb128_decode(data, secSize, offset);
					size_t arg2 = leb128_decode(data, secSize, offset);
					std::cout << "\t2 arguments with values: " << arg1 << " " << arg2 << std::endl;
					break;
				}
				case InstrKind::BR_TABLE:
				{
					size_t num_args = leb128_decode(data, secSize, offset);
					std::cout << "\t" << num_args << " arguments with values: ";
					size_t arg;
					for (int i = 0; i<num_args; i++) {
						arg = leb128_decode(data, secSize, offset);
						std::cout << arg << " ";
					}
					std::cout << std::endl;
					break;
				}
				case InstrKind::I32_LOAD:
				case InstrKind::I64_LOAD:
				case InstrKind::F32_LOAD:
				case InstrKind::F64_LOAD:
				case InstrKind::I32_LOAD8_S:
				case InstrKind::I32_LOAD8_U:
				case InstrKind::I32_LOAD16_S:
				case InstrKind::I32_LOAD16_U:
				case InstrKind::I64_LOAD8_S:
				case InstrKind::I64_LOAD8_U:
				case InstrKind::I64_LOAD16_S:
				case InstrKind::I64_LOAD16_U:
				case InstrKind::I64_LOAD32_S:
				case InstrKind::I64_LOAD32_U:
				case InstrKind::I32_STORE:
				case InstrKind::I64_STORE:
				case InstrKind::F32_STORE:
				case InstrKind::F64_STORE:
				case InstrKind::I32_STORE8:
				case InstrKind::I32_STORE16:
				case InstrKind::I64_STORE8:
				case InstrKind::I64_STORE16:
				case InstrKind::I64_STORE32:
				{
					size_t arg0 = leb128_decode(data, secSize, offset);
					size_t arg1 = leb128_decode(data, secSize, offset);
					if (arg0 >= 64 && arg0 < 128) {
						size_t arg2 = leb128_decode(data, secSize, offset);
						std::cout << "\t3 arguments with values: " << arg0 << " " << arg1 << " " << arg2 << std::endl;
					}
					else {
						std::cout << "\t2 arguments with values: " << arg0 << " " << arg1 << std::endl;
					}
					break;
				}
				case InstrKind::F32_CONST:
				{
					//Argument is IEEE7554 single point precision float stored in little endian format
					uint32_t bits = 0;
					for (int i = 0; i<=3; i++) {
						bits |= static_cast<uint32_t>(data[offset]) << 8*i;
						offset++;
					}
					float arg;
					std::memcpy(&arg, &bits, sizeof(float));
					std::cout << "\t1 argument with value: " << arg << std::endl;
					break;
				}
				case InstrKind::F64_CONST:
				{
					//Argument is IEEE7554 double point precision float stored in little endian format
					uint64_t bits = 0;
					for (int i = 0; i<=7; i++) {
						bits |= static_cast<uint64_t>(data[offset]) << 8*i;
						offset++;
					}
					double arg;
					std::memcpy(&arg, &bits, sizeof(double));
					std::cout << "\t1 argument with value: " << arg << std::endl;
					break;
				}
			}
		}
	}
}

/* This function is for decoding, not finished yet
void parse_code_section(std::span<const uint8_t> data, Module& module)  //incomplete
{
	size_t secSize = data.size();
	size_t offset = 0;
	size_t num_funcs = leb128_decode(data, secSize, offset);

	
	LocalData locals;
	size_t numlocals;
	
	for (int i = 0; i < num_funcs; i++) 
	{
		size_t end_of_body = offset + leb128_decode(data, secSize, offset); //size of function body
		numlocals = leb128_decode(data, secSize, offset);
		while (numlocals)
		{
			size_t numlocals_specifictype = leb128_decode(data, secSize, offset);
			numlocals -= numlocals_specifictype;
			offset++; //Skip over the type for now
						//What you really have to do here: Read next byte to figure out which data type the locals are, then add numlocals to the count of said data type in the locals struct
		}

		for (int j = 0; j < bodySize; j++) 
		{
			InstrKind opcode = binary_to_opcode.at(data[offset]);	//change this to Likith's static_cast to enum later
			Instruction instr = Instruction(opcode);
			
		}
		
	}
}

getOpcodeArgs(Instruction& instr, std::span<const uint8_t> data, size_t offset) {
	Instruction kind = instr.kind;
	switch (kind) {
		case InstrKind::BR:
		case InstrKind::BR_IF:
		case InstrKind::LOCAL_GET:
		case InstrKind::LOCAL_SET:
		case InstrKind::LOCAL_TEE:
		case InstrKind::GLOBAL_GET:
		case InstrKind::GLOBAL_SET:
		case InstrKind::MEMORY_SIZE:
		case InstrKind::MEMORY_GROW:
		case InstrKind::CALL:
		case InstrKind::RETURN_CALL:
			int64_t arg0 = static_cast<int64_t>leb128_decode(data, offset)
		case InstrKind::I32_CONST:
		case InstrKind::I64_CONST:
	}
}
*/

//TYPE SECTION ID=1
void parse_type_section(std::span<const uint8_t> data, Module& module)
{
	size_t secSize=data.size(); // size of the func
	size_t offset =0;
	uint32_t funcCount= leb128_decode(data,secSize,offset);
	std::cout<<"FUNCTION COUNT:"<<funcCount<<std::endl;
	while(offset<secSize)
	{
		if(data[offset++]!=0x60)
		{
			std::cout<<"Type tag mismacthed";
			return;
		}
		uint32_t pramCount=leb128_decode(data,secSize,offset);
		std::cout<<"Parameter count"<<pramCount<<std::endl;
		for(uint32_t i=0;i<pramCount;i++)
		{
			Type pType =static_cast<Type>(data[offset++]); 
			std::cout<<"PARAMETER TYPE OF "<<i<<(pType== Type::I32? " i32":" Other")<<std::endl;
		}
		uint32_t returnCount =leb128_decode(data,secSize,offset);
		std::cout<<"Return count"<<returnCount<<std::endl;
		for(uint32_t i=0;i<returnCount;i++)
		{
			Type rType =static_cast<Type>(data[offset++]); 
			std::cout<<"RETURN TYPE OF "<<i<<(rType== Type::I32? " i32":" Other")<<std::endl;
		}
	}
}
void parse_func_section(std::span<const uint8_t>data, Module &module)
{
	size_t secSize =data.size();
	size_t offset =0;
	size_t funcCount=leb128_decode(data,secSize,offset);
	std::cout<<"Number of functions:"<<funcCount<<std::endl;
	int i = 0;
	while(offset<secSize)
	{
		size_t index = leb128_decode(data,secSize,offset);
		std::cout<<"The function at index "<<i<<" uses the signature at index "<<index<<std::endl;
		i++;
	}

	
}
void parse_start_section(std::span<const uint8_t>data, Module &module)
{
	size_t secSize =data.size();
	size_t offset =0;
	size_t start_index =leb128_decode(data,secSize,offset);
	std::cout<<"Index of start function is: "<<start_index<<std::endl;
}
void parse_global_section(std::span<const uint8_t>data,Module& module)
{
	size_t secSize =data.size();
	size_t offset=0;
	size_t globalCount =leb128_decode(data,secSize,offset);
	std::cout<<"Number of globals are: "<<globalCount<<std::endl;
	for(uint32_t i=0;i<globalCount;i++)
	{
		Type gType =static_cast<Type>(data[offset++]); 
		std::cout<<"Type is: "<<(gType== Type::I32? " i32\n":" Other\n");
		bool mut =data[offset++];
		std::cout<<"mut: "<<mut<<std::endl;
		Type type =static_cast<Type>(data[offset++]); 
		std::cout<<"opcode is : "<<(type == Type::I32_const? " i32.const\n":" Other\n");
		size_t value = leb128_decode(data , secSize,offset); 
		std::cout<<"Value is : "<<value<<std::endl;
		offset++;
	}
}
void parse_mem_section(std::span<const uint8_t>data,Module& module)
{
	size_t secSize=data.size();
	size_t offset=0;
	size_t MemCount = leb128_decode(data,secSize,offset);
	bool hasMax =data[offset++];
	size_t min = leb128_decode(data,secSize,offset);
	size_t max;
	std::cout<<"Min: "<<min<<std::endl;
	if(hasMax)
	{
		max= leb128_decode(data,secSize,offset);
		std::cout<<"Max: "<<max<<std::endl;
	}
	
}
void parse_table_section(std::span<const uint8_t>data,Module& module)
{
	size_t secSize =data.size();
	size_t offset =0;
	size_t tableCount= leb128_decode(data,secSize,offset);
	for(size_t i=0;i<tableCount;i++)
	{
		uint8_t refType= data[offset++];
		std::cout<<"RefType: "<<(refType ==0x70? "funcref\n":"")<<(refType ==0x6F? "externref\n":"");
		bool hasMax = data[offset++];
		size_t min = leb128_decode(data,secSize,offset);
		size_t max;
		std::cout<<"Min: "<<min<<std::endl;
		if(hasMax)
		{
		max= leb128_decode(data,secSize,offset);
		std::cout<<"Max: "<<max<<std::endl;
		}
	}
}
void parse_import_section(std::span<const uint8_t>data, Module& module)
{
	size_t secSize = data.size();
	size_t offset = 0;
	size_t importCount = leb128_decode(data,secSize,offset);
	std::cout<<"Import Count: "<<importCount<<std::endl;
	for(size_t i =0; i<importCount; i++)
	{
		std::string modStr =read_string(data,offset);
		std::cout<<"Import from: "<<modStr<<std::endl;
		std::string feildStr=read_string(data,offset);
		std::cout<<"feild name: "<<feildStr<<std::endl;
		uint8_t descTag = data[offset++];
		switch (descTag)
		{
			case 0x00:
			{
				std::cout<<"Function"<<std::endl;
				size_t func_index = leb128_decode(data,secSize,offset);
				break;
			}
			case 0x01:
			{
				uint8_t refType= data[offset++];
				std::cout<<"RefType: "<<(refType ==0x70? "funcref\n":"")<<(refType ==0x6F? "externref\n":"");
				bool hasMax = data[offset++];
				size_t min = leb128_decode(data,secSize,offset);
				size_t max;
				std::cout<<"Min: "<<min<<std::endl;
				if(hasMax)
				{
					max= leb128_decode(data,secSize,offset);
					std::cout<<"Max: "<<max<<std::endl;
				}
				break;
			}
			case 0x02:
			{
				std::cout<<"Memory"<<std::endl;
				bool hasMax =data[offset++];
				size_t min = leb128_decode(data,secSize,offset);
				size_t max;
				std::cout<<"Min: "<<min<<std::endl;
				if(hasMax)
				{
					max= leb128_decode(data,secSize,offset);
					std::cout<<"Max: "<<max<<std::endl;
				}
				break;
			}
			case 0x03:
			{
				std::cout<<"Gloabal"<<std::endl;
				Type gType =static_cast<Type>(data[offset++]);
				std::cout<<"Type is: "<<(gType== Type::I32? " i32\n":" Other\n");
				bool mut =data[offset++];
				std::cout<<"mut: "<<mut<<std::endl;
				break;
			}
			default:
				break;
		
		}

	
	}
}
void parse_element_section(std::span<const uint8_t>data,Module& module)
{
	size_t secSize =data.size();
	size_t offset = 0;
	size_t subSecCount=leb128_decode(data,secSize,offset);
	for(size_t i =0;i<subSecCount;i++)
	{
		Flag flag =static_cast<Flag>(leb128_decode(data,secSize,offset));
		switch (flag)
		{
			case Flag::ActiveImplicit:
			case Flag::ActiveImplicitExpr:
			{
				Type opcode =static_cast<Type>(data[offset++]);
				std::cout<<"OPCODE"<<(opcode==Type::I32_const?" i32.const\n": " Other\n");
				size_t offsetVal=leb128_decode(data,secSize,offset);
				offset++;//skipping END Byte
				size_t eleCount =leb128_decode(data,secSize,offset);
   				std::cout << "Offset: "  << offsetVal<< std::endl;
   				std::cout << "Count: "   << eleCount << std::endl;
				if(flag==Flag::ActiveImplicit)
				parse_function(data,eleCount,offset,secSize,module);
				else if(flag==Flag::ActiveImplicitExpr)
				parse_expression(data,eleCount,offset,secSize,module);
				break;
				
			}
			case Flag::ActiveExplicit:
			case Flag::ActiveExplicitExpr:
			{
				size_t tableIND=leb128_decode(data,secSize,offset);
				Type opcode =static_cast<Type>(data[offset++]);
				std::cout<<"OPCODE"<<(opcode==Type::I32_const?"i32.const\n": "Other\n");
				offset++;//skipping END Byte
				uint8_t eleKind = data[offset++];//always 0x00
				std::cout<<"Element kind: "<<(eleKind ==0x00 ?"FuncRef" :"NONE");
				size_t eleCount = leb128_decode(data,secSize,offset);
				if(flag==Flag::ActiveExplicit)
				parse_function(data,eleCount,offset,secSize,module);
				else if(flag==Flag::ActiveExplicitExpr)
				parse_expression(data,eleCount,offset,secSize,module);
				break;
			}
			case Flag::Passive:
			case Flag::PassiveExpr:
			case Flag::DeclarativeExpr:
			case Flag::Declarative:
			{
				uint8_t eleKind =data[offset++];//always 0x00
				std::cout<<"Element kind: "<<(eleKind ==0x00 ?"FuncRef" :"NONE");
				size_t eleCount = leb128_decode(data,secSize,offset);
				if(flag==Flag::Passive||flag==Flag::Declarative)
				parse_function(data,eleCount,offset,secSize,module);
				else if(flag==Flag::DeclarativeExpr||flag==Flag::PassiveExpr)
				parse_expression(data,eleCount,offset,secSize,module);
				break;
			}
		default:
			break;
		}
		

	}
}
void parse_data_section(std::span<const uint8_t>data,Module& module)
{
	size_t secSize = data.size();
	size_t offset = 0;
	size_t dataCount=leb128_decode(data,secSize,offset);
	for(size_t i =0;i<dataCount;i++)
	{
		Flag flag =static_cast<Flag>(leb128_decode(data,secSize,offset));
		size_t byteCount=0;
		if(flag!=Flag::Passive)
		{
			if(flag==Flag::ActiveExplicit)
			size_t memIND=leb128_decode(data,secSize,offset);
			Type opcode =static_cast<Type>(data[offset++]);
			std::cout<<"OPCODE"<<(opcode==Type::I32_const?" i32.const\n": " Other\n");
			size_t offsetVal=leb128_decode(data,secSize,offset);
			offset++;//skipping END Byte
			byteCount=leb128_decode(data,secSize,offset);
		}
	
		for(size_t i=0; i<byteCount;i++)
		{
			uint8_t byte=data[offset++];
			std::cout<<"BYTE "<<byte<<std::endl;
		}

	}
}
void parse_export_section(std::span<const uint8_t>data,Module& module)
{
	size_t secSize=data.size();
	size_t offset =0;
	size_t exportCount=leb128_decode(data,secSize,offset);
	for(size_t i=0;i<exportCount;i++)
	{
		std::string name =read_string(data ,offset);
		std::cout<<"Export Name: "<<name<<std::endl;
		Tag tag =static_cast<Tag>(data[offset++]);
		std::cout<<"Tag "<<(tag==Tag::Function? "Function\n":"Other\n");
		size_t index=leb128_decode(data,secSize,offset);
		std::cout<<"Index "<<index<<std::endl;
	}
}


//Helpers
std::vector<uint8_t> Loadfile(std::string Path)
{
	//open the file in binary mode, get its size
    std::ifstream file(Path, std::ios::binary);
    file.seekg(0, std::ios::end);
    int size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    

    if ( !(data[0] == 0x00 && data[1] == 0x61 && data[2] == 0x73 && data[3] == 0x6D
		&& data[4] == 0x01 && data[5] == 0x00 && data[6] == 0x00 && data[7] == 0x00) )
    {
		return std::vector<uint8_t>(); //if incorrect file type, return empty vector (empty data vector = error opening file)
    }

	return data; //otherwise return data
}

void parse_function(std::span<const uint8_t>data,size_t eleCount,size_t &offset,size_t secSize,Module& module)
{
	for(size_t i=0;i<eleCount;i++)
	{
		size_t func_ID = leb128_decode(data,secSize,offset);
		std::cout << "func index: " << func_ID << std::endl;
	}
}
void parse_expression(std::span<const uint8_t>data,size_t eleCount,size_t &offset,size_t secSize,Module& module)
{
	uint8_t opcode = leb128_decode(data,secSize,offset);
	switch (opcode)
	{
	case 0xD2: //ref.func
	{
		size_t funcIND=leb128_decode(data,secSize,offset);
		std::cout << "ref.func: " << funcIND<< std::endl;
		offset++;//skipping END Byte(END BYTE = 0x0B)
		break;
	}
	case 0xD0:
	{
		//ref.null
		uint8_t reftype =data[offset++];//can be 0x70||0x6F
		std::cout << "ref.null refType: " << (reftype ==0x70 ?"funcRef" :"externref")<< std::endl;
		offset++;
		break;
	}
	case 0x23:
	{
		size_t globalIND=leb128_decode(data,secSize,offset);
		std::cout<< "global.get: " << globalIND << std::endl;
		offset++;
		break;
	}
	default:
		break;
	}
}
std::string read_string(std::span<const uint8_t>data,size_t &offset)
{
	size_t secSize=data.size();
	size_t len =leb128_decode(data,secSize,offset);
	std::cout<<"Length: "<<len<<std::endl;
	std::string bytes ="";
	for(size_t j=0;j<len;j++)
	{
		bytes +=(char)(data[offset++]);
	}
	return bytes;
}

void print_string_of_opcode(InstrKind opcode) {
	switch (opcode) {
		case InstrKind::UNREACHABLE:
			std::cout << "UNREACHABLE" << std::endl; break;
		case InstrKind::NOP:
			std::cout << "NOP" << std::endl; break;
		case InstrKind::BLOCK:
			std::cout << "BLOCK" << std::endl; break;
		case InstrKind::LOOP:
			std::cout << "LOOP" << std::endl; break;
		case InstrKind::IF:
			std::cout << "IF" << std::endl; break;
		case InstrKind::ELSE:
			std::cout << "ELSE" << std::endl; break;
		case InstrKind::END:
			std::cout << "END" << std::endl; break;
		case InstrKind::BR:
			std::cout << "BR" << std::endl; break;
		case InstrKind::BR_IF:
			std::cout << "BR_IF" << std::endl; break;
		case InstrKind::BR_TABLE:
			std::cout << "BR_TABLE" << std::endl; break;
		case InstrKind::RETURN:
			std::cout << "RETURN" << std::endl; break;
		case InstrKind::CALL:
			std::cout << "CALL" << std::endl; break;
		case InstrKind::CALL_INDIRECT:
			std::cout << "CALL_INDIRECT" << std::endl; break;
		case InstrKind::DROP:
			std::cout << "DROP" << std::endl; break;
		case InstrKind::SELECT:
			std::cout << "SELECT" << std::endl; break;
		case InstrKind::LOCAL_GET:
			std::cout << "LOCAL_GET" << std::endl; break;
		case InstrKind::LOCAL_SET:
			std::cout << "LOCAL_SET" << std::endl; break;
		case InstrKind::LOCAL_TEE:
			std::cout << "LOCAL_TEE" << std::endl; break;
		case InstrKind::GLOBAL_GET:
			std::cout << "GLOBAL_GET" << std::endl; break;
		case InstrKind::GLOBAL_SET:
			std::cout << "GLOBAL_SET" << std::endl; break;
		case InstrKind::I32_LOAD:
			std::cout << "I32_LOAD" << std::endl; break;
		case InstrKind::I64_LOAD:
			std::cout << "I64_LOAD" << std::endl; break;
		case InstrKind::F32_LOAD:
			std::cout << "F32_LOAD" << std::endl; break;
		case InstrKind::F64_LOAD:
			std::cout << "F64_LOAD" << std::endl; break;
		case InstrKind::I32_LOAD8_S:
			std::cout << "I32_LOAD8_S" << std::endl; break;
		case InstrKind::I32_LOAD8_U:
			std::cout << "I32_LOAD8_U" << std::endl; break;
		case InstrKind::I32_LOAD16_S:
			std::cout << "I32_LOAD16_S" << std::endl; break;
		case InstrKind::I32_LOAD16_U:
			std::cout << "I32_LOAD16_U" << std::endl; break;
		case InstrKind::I64_LOAD8_S:
			std::cout << "I64_LOAD8_S" << std::endl; break;
		case InstrKind::I64_LOAD8_U:
			std::cout << "I64_LOAD8_U" << std::endl; break;
		case InstrKind::I64_LOAD16_S:
			std::cout << "I64_LOAD16_S" << std::endl; break;
		case InstrKind::I64_LOAD16_U:
			std::cout << "I64_LOAD16_U" << std::endl; break;
		case InstrKind::I64_LOAD32_S:
			std::cout << "I64_LOAD32_S" << std::endl; break;
		case InstrKind::I64_LOAD32_U:
			std::cout << "I64_LOAD32_U" << std::endl; break;
		case InstrKind::I32_STORE:
			std::cout << "I32_STORE" << std::endl; break;
		case InstrKind::I64_STORE:
			std::cout << "I64_STORE" << std::endl; break;
		case InstrKind::F32_STORE:
			std::cout << "F32_STORE" << std::endl; break;
		case InstrKind::F64_STORE:
			std::cout << "F64_STORE" << std::endl; break;
		case InstrKind::I32_STORE8:
			std::cout << "I32_STORE8" << std::endl; break;
		case InstrKind::I32_STORE16:
			std::cout << "I32_STORE16" << std::endl; break;
		case InstrKind::I64_STORE8:
			std::cout << "I64_STORE8" << std::endl; break;
		case InstrKind::I64_STORE16:
			std::cout << "I64_STORE16" << std::endl; break;
		case InstrKind::I64_STORE32:
			std::cout << "I64_STORE32" << std::endl; break;
		case InstrKind::MEMORY_SIZE:
			std::cout << "MEMORY_SIZE" << std::endl; break;
		case InstrKind::MEMORY_GROW:
			std::cout << "MEMORY_GROW" << std::endl; break;
		case InstrKind::I32_CONST:
			std::cout << "I32_CONST" << std::endl; break;
		case InstrKind::I64_CONST:
			std::cout << "I64_CONST" << std::endl; break;
		case InstrKind::F32_CONST:
			std::cout << "F32_CONST" << std::endl; break;
		case InstrKind::F64_CONST:
			std::cout << "F64_CONST" << std::endl; break;
		case InstrKind::I32_EQZ:
			std::cout << "I32_EQZ" << std::endl; break;
		case InstrKind::I32_EQ:
			std::cout << "I32_EQ" << std::endl; break;
		case InstrKind::I32_NE:
			std::cout << "I32_NE" << std::endl; break;
		case InstrKind::I32_LT_S:
			std::cout << "I32_LT_S" << std::endl; break;
		case InstrKind::I32_LT_U:
			std::cout << "I32_LT_U" << std::endl; break;
		case InstrKind::I32_GT_S:
			std::cout << "I32_GT_S" << std::endl; break;
		case InstrKind::I32_GT_U:
			std::cout << "I32_GT_U" << std::endl; break;
		case InstrKind::I32_LE_S:
			std::cout << "I32_LE_S" << std::endl; break;
		case InstrKind::I32_LE_U:
			std::cout << "I32_LE_U" << std::endl; break;
		case InstrKind::I32_GE_S:
			std::cout << "I32_GE_S" << std::endl; break;
		case InstrKind::I32_GE_U:
			std::cout << "I32_GE_U" << std::endl; break;
		case InstrKind::I64_EQZ:
			std::cout << "I64_EQZ" << std::endl; break;
		case InstrKind::I64_EQ:
			std::cout << "I64_EQ" << std::endl; break;
		case InstrKind::I64_NE:
			std::cout << "I64_NE" << std::endl; break;
		case InstrKind::I64_LT_S:
			std::cout << "I64_LT_S" << std::endl; break;
		case InstrKind::I64_LT_U:
			std::cout << "I64_LT_U" << std::endl; break;
		case InstrKind::I64_GT_S:
			std::cout << "I64_GT_S" << std::endl; break;
		case InstrKind::I64_GT_U:
			std::cout << "I64_GT_U" << std::endl; break;
		case InstrKind::I64_LE_S:
			std::cout << "I64_LE_S" << std::endl; break;
		case InstrKind::I64_LE_U:
			std::cout << "I64_LE_U" << std::endl; break;
		case InstrKind::I64_GE_S:
			std::cout << "I64_GE_S" << std::endl; break;
		case InstrKind::I64_GE_U:
			std::cout << "I64_GE_U" << std::endl; break;
		case InstrKind::F32_EQ:
			std::cout << "F32_EQ" << std::endl; break;
		case InstrKind::F32_NE:
			std::cout << "F32_NE" << std::endl; break;
		case InstrKind::F32_LT:
			std::cout << "F32_LT" << std::endl; break;
		case InstrKind::F32_GT:
			std::cout << "F32_GT" << std::endl; break;
		case InstrKind::F32_LE:
			std::cout << "F32_LE" << std::endl; break;
		case InstrKind::F32_GE:
			std::cout << "F32_GE" << std::endl; break;
		case InstrKind::F64_EQ:
			std::cout << "F64_EQ" << std::endl; break;
		case InstrKind::F64_NE:
			std::cout << "F64_NE" << std::endl; break;
		case InstrKind::F64_LT:
			std::cout << "F64_LT" << std::endl; break;
		case InstrKind::F64_GT:
			std::cout << "F64_GT" << std::endl; break;
		case InstrKind::F64_LE:
			std::cout << "F64_LE" << std::endl; break;
		case InstrKind::F64_GE:
			std::cout << "F64_GE" << std::endl; break;
		case InstrKind::I32_CLZ:
			std::cout << "I32_CLZ" << std::endl; break;
		case InstrKind::I32_CTZ:
			std::cout << "I32_CTZ" << std::endl; break;
		case InstrKind::I32_POPCNT:
			std::cout << "I32_POPCNT" << std::endl; break;
		case InstrKind::I32_ADD:
			std::cout << "I32_ADD" << std::endl; break;
		case InstrKind::I32_SUB:
			std::cout << "I32_SUB" << std::endl; break;
		case InstrKind::I32_MUL:
			std::cout << "I32_MUL" << std::endl; break;
		case InstrKind::I32_DIV_S:
			std::cout << "I32_DIV_S" << std::endl; break;
		case InstrKind::I32_DIV_U:
			std::cout << "I32_DIV_U" << std::endl; break;
		case InstrKind::I23_REM_S:
			std::cout << "I23_REM_S" << std::endl; break;
		case InstrKind::I32_REM_U:
			std::cout << "I32_REM_U" << std::endl; break;
		case InstrKind::I32_AND:
			std::cout << "I32_AND" << std::endl; break;
		case InstrKind::I32_OR:
			std::cout << "I32_OR" << std::endl; break;
		case InstrKind::I32_XOR:
			std::cout << "I32_XOR" << std::endl; break;
		case InstrKind::I32_SHL:
			std::cout << "I32_SHL" << std::endl; break;
		case InstrKind::I32_SHR_S:
			std::cout << "I32_SHR_S" << std::endl; break;
		case InstrKind::I32_SHR_U:
			std::cout << "I32_SHR_U" << std::endl; break;
		case InstrKind::I32_ROTL:
			std::cout << "I32_ROTL" << std::endl; break;
		case InstrKind::I32_ROTR:
			std::cout << "I32_ROTR" << std::endl; break;
		case InstrKind::I64_CLZ:
			std::cout << "I64_CLZ" << std::endl; break;
		case InstrKind::I64_CTZ:
			std::cout << "I64_CTZ" << std::endl; break;
		case InstrKind::I64_POPCNT:
			std::cout << "I64_POPCNT" << std::endl; break;
		case InstrKind::I64_ADD:
			std::cout << "I64_ADD" << std::endl; break;
		case InstrKind::I64_SUB:
			std::cout << "I64_SUB" << std::endl; break;
		case InstrKind::I64_MUL:
			std::cout << "I64_MUL" << std::endl; break;
		case InstrKind::I64_DIV_S:
			std::cout << "I64_DIV_S" << std::endl; break;
		case InstrKind::I64_DIV_U:
			std::cout << "I64_DIV_U" << std::endl; break;
		case InstrKind::I64_REM_S:
			std::cout << "I64_REM_S" << std::endl; break;
		case InstrKind::I64_REM_U:
			std::cout << "I64_REM_U" << std::endl; break;
		case InstrKind::I64_AND:
			std::cout << "I64_AND" << std::endl; break;
		case InstrKind::I64_OR:
			std::cout << "I64_OR" << std::endl; break;
		case InstrKind::I64_XOR:
			std::cout << "I64_XOR" << std::endl; break;
		case InstrKind::I64_SHL:
			std::cout << "I64_SHL" << std::endl; break;
		case InstrKind::I64_SHR_S:
			std::cout << "I64_SHR_S" << std::endl; break;
		case InstrKind::I64_SHR_U:
			std::cout << "I64_SHR_U" << std::endl; break;
		case InstrKind::I64_ROTL:
			std::cout << "I64_ROTL" << std::endl; break;
		case InstrKind::I64_ROTR:
			std::cout << "I64_ROTR" << std::endl; break;
		case InstrKind::F32_ABS:
			std::cout << "F32_ABS" << std::endl; break;
		case InstrKind::F32_NEG:
			std::cout << "F32_NEG" << std::endl; break;
		case InstrKind::F32_CEIL:
			std::cout << "F32_CEIL" << std::endl; break;
		case InstrKind::F32_FLOOR:
			std::cout << "F32_FLOOR" << std::endl; break;
		case InstrKind::F32_TRUNC:
			std::cout << "F32_TRUNC" << std::endl; break;
		case InstrKind::F32_NEAREST:
			std::cout << "F32_NEAREST" << std::endl; break;
		case InstrKind::F32_SQRT:
			std::cout << "F32_SQRT" << std::endl; break;
		case InstrKind::F32_ADD:
			std::cout << "F32_ADD" << std::endl; break;
		case InstrKind::F32_SUB:
			std::cout << "F32_SUB" << std::endl; break;
		case InstrKind::F32_MUL:
			std::cout << "F32_MUL" << std::endl; break;
		case InstrKind::F32_DIV:
			std::cout << "F32_DIV" << std::endl; break;
		case InstrKind::F32_MIN:
			std::cout << "F32_MIN" << std::endl; break;
		case InstrKind::F32_MAX:
			std::cout << "F32_MAX" << std::endl; break;
		case InstrKind::F32_COPYSIGN:
			std::cout << "F32_COPYSIGN" << std::endl; break;
		case InstrKind::F64_ABS:
			std::cout << "F64_ABS" << std::endl; break;
		case InstrKind::F64_NEG:
			std::cout << "F64_NEG" << std::endl; break;
		case InstrKind::F64_CEIL:
			std::cout << "F64_CEIL" << std::endl; break;
		case InstrKind::F64_FLOOR:
			std::cout << "F64_FLOOR" << std::endl; break;
		case InstrKind::F64_TRUNC:
			std::cout << "F64_TRUNC" << std::endl; break;
		case InstrKind::F64_NEAREST:
			std::cout << "F64_NEAREST" << std::endl; break;
		case InstrKind::F64_SQRT:
			std::cout << "F64_SQRT" << std::endl; break;
		case InstrKind::F64_ADD:
			std::cout << "F64_ADD" << std::endl; break;
		case InstrKind::F64_SUB:
			std::cout << "F64_SUB" << std::endl; break;
		case InstrKind::F64_MUL:
			std::cout << "F64_MUL" << std::endl; break;
		case InstrKind::F64_DIV:
			std::cout << "F64_DIV" << std::endl; break;
		case InstrKind::F64_MIN:
			std::cout << "F64_MIN" << std::endl; break;
		case InstrKind::F64_MAX:
			std::cout << "F64_MAX" << std::endl; break;
		case InstrKind::F64_COPYSIGN:
			std::cout << "F64_COPYSIGN" << std::endl; break;
		case InstrKind::I32_WRAP_I64:
			std::cout << "I32_WRAP_I64" << std::endl; break;
		case InstrKind::I32_TRUNC_F32_S:
			std::cout << "I32_TRUNC_F32_S" << std::endl; break;
		case InstrKind::I32_TRUNC_F32_U:
			std::cout << "I32_TRUNC_F32_U" << std::endl; break;
		case InstrKind::I32_TRUNC_F64_S:
			std::cout << "I32_TRUNC_F64_S" << std::endl; break;
		case InstrKind::I32_TRUNC_F64_U:
			std::cout << "I32_TRUNC_F64_U" << std::endl; break;
		case InstrKind::I64_EXTEND_I32_S:
			std::cout << "I64_EXTEND_I32_S" << std::endl; break;
		case InstrKind::I64_EXTEND_I32_U:
			std::cout << "I64_EXTEND_I32_U" << std::endl; break;
		case InstrKind::I64_TRUNC_F32_S:
			std::cout << "I64_TRUNC_F32_S" << std::endl; break;
		case InstrKind::I64_TRUNC_F32_U:
			std::cout << "I64_TRUNC_F32_U" << std::endl; break;
		case InstrKind::I64_TRUNC_F64_S:
			std::cout << "I64_TRUNC_F64_S" << std::endl; break;
		case InstrKind::I64_TRUNC_F64_U:
			std::cout << "I64_TRUNC_F64_U" << std::endl; break;
		case InstrKind::F32_CONVERT_I32_S:
			std::cout << "F32_CONVERT_I32_S" << std::endl; break;
		case InstrKind::F32_CONVERT_I32_U:
			std::cout << "F32_CONVERT_I32_U" << std::endl; break;
		case InstrKind::F32_CONVERT_I64_S:
			std::cout << "F32_CONVERT_I64_S" << std::endl; break;
		case InstrKind::F32_CONVERT_I64_U:
			std::cout << "F32_CONVERT_I64_U" << std::endl; break;
		case InstrKind::F32_DEMOTE_F64:
			std::cout << "F32_DEMOTE_F64" << std::endl; break;
		case InstrKind::F64_CONVERT_I32_S:
			std::cout << "F64_CONVERT_I32_S" << std::endl; break;
		case InstrKind::F64_CONVERT_I32_U:
			std::cout << "F64_CONVERT_I32_U" << std::endl; break;
		case InstrKind::F64_CONVERT_I64_S:
			std::cout << "F64_CONVERT_I64_S" << std::endl; break;
		case InstrKind::F64_CONVERT_I64_U:
			std::cout << "F64_CONVERT_I64_U" << std::endl; break;
		case InstrKind::F64_PROMOTE_F32:
			std::cout << "F64_PROMOTE_F32" << std::endl; break;
		case InstrKind::I32_REINTERPRET_F32:
			std::cout << "I32_REINTERPRET_F32" << std::endl; break;
		case InstrKind::I64_REINTERPRET_F64:
			std::cout << "I64_REINTERPRET_F64" << std::endl; break;
		case InstrKind::F32_REINTERPRET_I32:
			std::cout << "F32_REINTERPRET_I32" << std::endl; break;
		case InstrKind::F64_REINTERPRET_I64:
			std::cout << "F64_REINTERPRET_I64" << std::endl; break;
		default:
			std::cout << "Invalid opcode" << std::endl;
	}
}

size_t leb128_decode(std::span<const uint8_t> data, size_t size, size_t& offset)
{																			
    size_t integer=0;
    
    int shift=0;
    while(offset<size)													
    {
        if(data[offset]&0x80) 
        {
            integer|=(data[offset]&0x7f)<<shift;
            shift+=7;
        }
        else{
            integer|=(data[offset]&0x7f)<<shift; 
			++offset; //move offset past the end of the integer before returning
            return integer;
        }
		++offset;
    }
    return integer;
}

