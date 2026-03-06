#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<span>
#include "module/module.hpp"
#include "parser/parser.hpp" 

//Section parsing
void parse_code_section(std::span<const uint8_t> data, Module& module)  //incomplete
{
	int funcIndex = 0;
	size_t secSize = data.size();
	size_t offset = 0;
	size_t num_funcs = leb128_decode(data, secSize, offset);
	
	struct LocalData //not sure what to do with local variable data counts for now so I'm just gonna leave it here
	{ 
		int i32_count;
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
	
	LocalData locals;
	size_t numlocals;
	
	for (int i = 0; i < num_funcs; i++) 
	{
		leb128_decode(data, secSize, offset); //skip over the size of the function body
		numlocals = leb128_decode(data, secSize, offset);
		while (numlocals)
		{
			size_t numlocals_specifictype = leb128_decode(data, secSize, offset);
			numlocals -= numlocals_specifictype;
			//Read next byte to figure out which data type the locals are, then add numlocals to the count of said data type in the locals struct
		}
	}
}
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
			Type pType =Hex_to_type(data[offset++]);
			std::cout<<"PARAMETER TYPE OF "<<i<<(pType== Type::I32? " i32":" Other")<<std::endl;
		}
		uint32_t returnCount =leb128_decode(data,secSize,offset);
		std::cout<<"Return count"<<returnCount<<std::endl;
		for(uint32_t i=0;i<returnCount;i++)
		{
			Type rType =Hex_to_type(data[offset++]);
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
	int i =0;
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
		Type gType =Hex_to_type(data[offset++]);
		std::cout<<"Type is: "<<(gType== Type::I32? " i32\n":" Other\n");
		bool mut =data[offset++];
		std::cout<<"mut: "<<mut<<std::endl;
		Type type=Hex_to_type(data[offset++]); //Use InstrKind for this 
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
				Type gType =Hex_to_type(data[offset++]);
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
				uint8_t offsetO=data[offset++];
				Type opcode = Hex_to_type(offsetO);
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
				uint8_t offsetO=data[offset++];
				Type opcode = Hex_to_type(offsetO);
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
			uint8_t offsetO=data[offset++];
			Type opcode = Hex_to_type(offsetO);
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

Type Hex_to_type(uint8_t byte)
{
	switch (byte)
    {
    case 0x7F:
        return Type::I32;
    case 0x7E:
        return Type::I64;
    case 0x7D:
        return Type::F32;
    case 0x7C:
        return Type::F64;
	case 0x41:
		return Type::I32_const;
	case 0x42:
		return Type::I64_const;
	case 0x43:
		return Type::F32_const;
	case 0x44:
		return Type::F64_const;
	case 0x23:
		return Type::Global_get;
    default:
        std::cout<<"Invalid Type"<<std::endl;
		return Type::NONE;
    }
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
	size_t secSize = data.size();
	size_t len =leb128_decode(data,secSize,offset);
	std::cout<<"Length: "<<len<<std::endl;
	std::string bytes ="";
	for(size_t j=0;j<len;j++)
	{
		bytes +=(char)(data[offset++]);
	}
	return bytes;
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

