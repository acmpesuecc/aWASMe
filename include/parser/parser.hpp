#pragma once
#include<string>
#include<span>
#include<unordered_map>
#include "module/module.hpp"
#include "parser/codeparsing.hpp"
#include "parser/types.hpp" 

//Section parsing 
int64_t parse_code_section(std::span<const uint8_t> data, Module& module);
int64_t parse_type_section(std::span<const uint8_t> data, Module& module);
int64_t parse_func_section(std::span<const uint8_t> data, Module& module, int64_t typeCount);
int parse_start_section(std::span<const uint8_t> data, Module& module);
int parse_mem_section(std::span<const uint8_t> data, Module& module);
int parse_import_section(std::span<const uint8_t> data, Module& module, int64_t typeCount);
int parse_global_section(std::span<const uint8_t> data, Module& module);
void parse_table_section(std::span<const uint8_t> data, Module& module);
void parse_element_section(std::span<const uint8_t> data, Module& module);
void parse_data_section(std::span<const uint8_t> data, Module& module);
int parse_export_section(std::span<const uint8_t> data, Module& module);

//Helpers
std::vector<uint8_t> Loadfile(std::string Path);
size_t leb128_decode(std::span<const uint8_t> data, size_t& offset, int num_bits, bool is_signed);
void print_string_of_opcode(Instr opcode);

//Helpers for ELEMENT SECTION
void parse_function(std::span<const uint8_t>data, size_t eleCount, size_t &offset, size_t secSize, Module& module);
void parse_expression(std::span<const uint8_t>data, size_t eleCount, size_t &offset, size_t secSize, Module& module);

//Helpers for IMPORT AND EXPORT 
std::string read_string(std::span<const uint8_t>data, size_t &offset);
