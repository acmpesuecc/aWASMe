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
	std::cout<<"FUNCTION COUNT??:"<<funcCount<<std::endl;
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
	

	//mapping ?? map hex to types TT
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
        break;
    case 0x7E:
        return Type::I64;
        break;
    case 0x7D:
        return Type::F32;
        break;
    case 0x7C:
        return Type::F64;
        break;
    default:
        return Type::NONE;
        std::cout<<"Invalid Type"<<std::endl;
        break;
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

