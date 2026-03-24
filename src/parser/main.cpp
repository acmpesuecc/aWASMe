/* To execute: 
	g++ -std=c++20 -I./include src/parser/main.cpp src/parser/parser.cpp src/parser/codeparsing.cpp -o parser.exe
	./parser.exe "<path to .wasm file>"

	(or)

	make run-parser ARGS="<path to .wasm file>"
		Note: this includes extra warnings -Wall -Wextra
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
		std::cout << "\tNote: If running through make command 'make run-parser', expected usage is: make run-parser ARGS=\"<path>\"" << std::endl;
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
		
        switch (Id)
        {
         // Call the corrosponding functions here 
            case 1:
				parse_type_section(sectionData,module);
                break;
			case 2:
				parse_import_section(sectionData,module);
				break;
            case 3:
				parse_func_section(sectionData,module);
                break;
			case 4:
				parse_table_section(sectionData,module);
				break;
			case 5:
				parse_mem_section(sectionData,module);
				break;
			case 6:
				parse_global_section(sectionData,module);
				break;
			case 7:
				parse_export_section(sectionData,module);
				break;
            case 8:
				parse_start_section(sectionData,module);
                break;
			case 9:
				parse_element_section(sectionData,module);
				break;
            case 10:
				parse_code_section(sectionData, module);	
                break;
			case 11:
				parse_data_section(sectionData,module);
				break;
            default:
                break;
        } 
		std::cout<<"--------------------------------------------------------------------------\n";
		
		offset+=secSize;
    }
	return 0;
}