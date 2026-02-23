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

