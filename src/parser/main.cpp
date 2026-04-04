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

int main (int argc, char* argv[]) 
{	
	std::cout << "\n****Note: all numeric values being printed on the screen are in hexadecimal format****" << std::endl;
	std::cout << std::hex;
	
	//Get file path and load the data
	if (argc != 2) 
	{
		std::cout << "Expected 1 file path as command line input" << std::endl;
		std::cout << "\tNote: If running through make command 'make run-parser', expected usage is: make run-parser ARGS=\"<path>\"" << std::endl;
		return 0;
	}
	
	//Check magic number and version
	std::vector<uint8_t> data = Loadfile(argv[1]);
	size_t filesize = data.size();
	if (filesize == 0) 
	{
		std::cout << "Invalid file given for parsing (not WASM 1.0). Aborting." << std::endl;
		return 0;
	}
	std::span<const uint8_t> dataspan = data;
	std::span<const uint8_t> sectionData;

	//Check whether all sections have proper section sizes and appear in order
	size_t offset = 8; //Bytes 0-7 have already been parsed by verifying the magic number and version
	int8_t previous_sec_id = -1;
	size_t secSize;

	std::cout<<"--------------------------------------------------------------------------" << std::endl;	
	std::cout << "Scanning section IDs... " << std::endl;
	while (offset < filesize)
	{
		std::cout << "ID: '" << (int)data[offset] << "' ";
		if (data[offset] > 0x0D)
		{
			std::cout << "Invalid." << std::endl;
			std::cout << "\tNote: This may be because the section sizes are corrupted." << std::endl;
			std::cout << "Aborting." << std::endl;
			return 0;
		}
		if (previous_sec_id >= data[offset])
		{
			std::cout << "Section IDs are out of order. Aborting." << std::endl;
			return 0;
		}
		std::cout << "OK" << std::endl;
		++offset;
		secSize = leb128_decode(data, offset, 32, false);
		offset += secSize;
	}
	std::cout << "Section IDs and sizes validated. Starting parsing." << std::endl;
	std::cout<<"--------------------------------------------------------------------------" << std::endl;
	std::cout<<"--------------------------------------------------------------------------" << std::endl;
	
	offset = 8; //reset offset
	Module module{};
	int64_t typeCount;
	int64_t funcCount;
	int64_t codeCount;

    while (offset < filesize)
    {
        uint8_t Id = data[offset]; //read section ID
		std::cout << "Section ID: " << std::dec<<(int)Id << std::endl;
		++offset;
		secSize = leb128_decode(dataspan, offset, 32, false); //read section size - leb128 changes offset globally to be past the integer by itself 
		std::cout << "Section size in bytes: " << secSize << std::endl;
		sectionData = dataspan.subspan(offset, secSize);		//bc of above, section size is not included in section data when passing to parsing functions
		
        switch (Id)
        {
			case 0: 
			{
				std::cout << "Custom section detected. Skipping" << std::endl;
				break;
			}
            case 1:
			{
				std::cout << "Parsing type section" << std::endl;
				typeCount = parse_type_section(sectionData, module);
                break;
			}
			case 2:
			{
				std::cout << "Parsing import section" << std::endl;
				parse_import_section(sectionData, module, typeCount);
				break;
			}
            case 3:
			{
				std::cout << "Parsing function section" << std::endl;
				funcCount = parse_func_section(sectionData, module, typeCount);
				if (funcCount == -1)
				{
					std::cout << "Error occured while parsing function section. Aborting." << std::endl;
					return 0;
				}
                break;
			}
			case 4:
			{
				std::cout << "Parsing table section" << std::endl;
				parse_table_section(sectionData, module);
				break;
			}
			case 5:
			{
				std::cout << "Parsing memory section" << std::endl;
				parse_mem_section(sectionData, module);
				break;
			}
			case 6:
			{
				std::cout << "Parsing global section" << std::endl;
				parse_global_section(sectionData, module);
				break;
			}
			case 7:
			{
				std::cout << "Parsing export section" << std::endl;
				parse_export_section(sectionData, module);
				break;
			}
            case 8:
			{
				std::cout << "Parsing start section" << std::endl;
				int err = parse_start_section(sectionData, module);
				if (err == -1)
				{
					std::cout << "Error parsing start section. Aborting." << std::endl;
					return 0;
				}
                break;
			}
			case 9:
			{
				std::cout << "Parsing element section" << std::endl;
				parse_element_section(sectionData, module);
				break;
			}
            case 10:
			{
				std::cout << "Parsing code section" << std::endl;
				codeCount = parse_code_section(sectionData, module);
				if (codeCount == -1)
				{
					std::cout << "Error occured while parsing code section. Aborting." << std::endl;
					return 0;
				}
				if (funcCount != codeCount)
				{
					std::cout << "Error - element count of function and type section do not match." << std::endl;
					std::cout << "\tFunction section - number of functions mapped to types: " << funcCount << std::endl;
					std::cout << "\tCode section - function body count: " << codeCount << std::endl;
					std::cout << "Aborting." << std::endl;
					return 0;
				}
                break;
			}
			case 11:
			{
				std::cout << "Parsing data section" << std::endl;
				parse_data_section(sectionData, module);
				break;
			}
            default:
                break;
        } 
		std::cout<<"--------------------------------------------------------------------------" << std::endl;
		
		offset += secSize;
    }
	validate_Function(module.functions,module.types);
	return 0;
}