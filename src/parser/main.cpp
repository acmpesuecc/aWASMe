/* To execute: 
	g++ -std=c++20 -I./include src/parser/main.cpp src/parser/parser.cpp src/parser/codeparsing.cpp src/parser/stackValidator.cpp -o parser.exe
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
#include "parser/stackValidator.hpp"

int main (int argc, char* argv[]) {
	
	if (argc != 2) 
	{
		std::cout << "Expected 1 file path as command line input" << std::endl;
		std::cout << "\tNote: If running through make command 'make run-parser', expected usage is: make run-parser ARGS=\"<path>\"" << std::endl;
		return 0;
	}
	
	std::vector<uint8_t> data = Loadfile(argv[1]);
	size_t filesize = data.size();
	if (filesize == 0) 
	{
		std::cout << "Invalid file given for parsing (not WASM 1.0)" << std::endl;
		return 0;
	}
	
	size_t offset = 8; //Bytes 0-7 have already been parsed by verifying the magic number and version
	std::span<const uint8_t> dataspan = data;
	std::span<const uint8_t> sectionData;
	size_t secSize;
	
	Module module{};
	
	std::cout << "****Note: all numeric values being printed on the screen are being printed in hexadecimal format\n\n" << std::endl;
	std::cout << std::hex;

    while (offset < filesize)
    {
        uint8_t Id = data[offset]; //read section ID
		std::cout << "Section ID: " << std::dec<<(int)Id << std::endl;
		++offset;
		secSize = leb128_decode(dataspan, filesize, offset); //read section size - leb128 changes offset globally to be past the integer by itself 
		std::cout << "Section size (read from wasm file): " << secSize << std::endl;
		sectionData = dataspan.subspan(offset, secSize);		//bc of above, section size is not included in section data when passing to parsing functions
		
        switch (Id)
        {
         // Call the corrosponding functions here 
            case 1:
				std::cout << "Parsing type section" << std::endl;
				parse_type_section(sectionData, module);
                break;
			case 2:
				std::cout << "Parsing import section" << std::endl;
				parse_import_section(sectionData, module);
				break;
            case 3:
				std::cout << "Parsing function section" << std::endl;
				parse_func_section(sectionData, module);
                break;
			case 4:
				std::cout << "Parsing table section" << std::endl;
				parse_table_section(sectionData, module);
				break;
			case 5:
				std::cout << "Parsing memory section" << std::endl;
				parse_mem_section(sectionData, module);
				break;
			case 6:
				std::cout << "Parsing global section" << std::endl;
				parse_global_section(sectionData, module);
				break;
			case 7:
				std::cout << "Parsing export section" << std::endl;
				parse_export_section(sectionData, module);
				break;
            case 8:
				std::cout << "Parsing start section" << std::endl;
				parse_start_section(sectionData, module);
                break;
			case 9:
				std::cout << "Parsing element section" << std::endl;
				parse_element_section(sectionData, module);
				break;
            case 10:
				std::cout << "Parsing code section" << std::endl;
				std::cout << "OFFSET BEFORE CODE SECTION: " << std::hex << (int)offset << std::endl; 
				parse_code_section(sectionData, module);	
                break;
			case 11:
				std::cout << "Parsing data section" << std::endl;
				parse_data_section(sectionData, module);
				break;
            default:
                break;
        } 
		std::cout<<"--------------------------------------------------------------------------" << std::endl;
		
		offset+=secSize;
    }
	validate_Function(module.functions,module.types);
	return 0;
}