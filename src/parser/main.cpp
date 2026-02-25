/* To execute: 
	g++ -std=c++20 -I./include src/parser/main.cpp src/parser/parser.cpp -o main./main
*/

#include<iostream>
#include<fstream>
#include<vector>
#include<cstdint>
#include<span>
#include "module/module.hpp"
#include "parser/parser.hpp" 

int main (int argc, char* argv[]) {
	
	if (argc != 2) 
	{
		std::cout << "Expected 1 file path as command line input" << std::endl;
		return 0;
	}
	
	std::vector<uint8_t> data = Loadfile(argv[1]);
	int filesize = data.size();
	if (filesize == 0) 
	{
		std::cout << "Invalid file given for parsing (not WASM 1.0)" << std::endl;
		return 0;
	}
	
	size_t offset = 8; //You have already parsed bytes 0-7 by verifying the magic number and version
	std::span<const uint8_t> dataspan = data;
	std::span<const uint8_t> sectionData;
	size_t secSize;
	
	Module module(filesize);
	std::cout << "Empty module created, file size is " << module.filesize << " bytes." << std::endl;
	
    while(offset<filesize)
    {
        uint8_t Id = data[offset]; //read section ID
		std::cout << "Section ID: " << (int)Id << std::endl;
		++offset;
		secSize = leb128_decode(data, filesize, offset); //read section size - leb128 changes offset globally to be past the integer by itself 
		std::cout << "Section size (read from wasm file): " << secSize << std::endl;
		sectionData = dataspan.subspan(offset, secSize);		//bc of above, section size is not included in section data when passing to parsing functions
		std::cout << "Section data size (size of program's copy of the section data): " << sectionData.size() << std::endl;
		
        switch (Id)
        {
         // Call the corrosponding functions here 
            case 1:
				parse_type_section(sectionData,module);
                break;
            case 3:
                break;
            case 8:
                break;
            case 10:
				//parse_code_section(sectionData, module);	
                break;
            default:
                break;
        } 
		
		offset+=secSize;
    }
	return 0;
}