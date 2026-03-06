#pragma once
#include<string>
#include<span>
#include "module/module.hpp"

//Section parsing 
void parse_code_section(std::span<const uint8_t> data, Module& module);
void parse_type_section(std::span<const uint8_t> data ,Module& module);
void parse_func_section(std::span<const uint8_t>data ,Module& module);
void parse_start_section(std::span<const uint8_t>data, Module& module);
void parse_mem_section(std::span<const uint8_t>data,Module& module);
void parse_import_section(std::span<const uint8_t>data, Module& module);
void parse_global_section(std::span<const uint8_t>data,Module& module);
//Helpers
std::vector<uint8_t> Loadfile(std::string Path);
size_t leb128_decode(std::span<const uint8_t> data, size_t size, size_t& offset);
//Helpers for IMPORT AND EXPORT 
std::string read_string(std::span<const uint8_t>data,size_t &offset);
//Data table - mapping binary instructions to their opcodes
//mapping for TYPE SECTION 
enum class Type{
    I32,
    I64,
    F32,
    F64,
    I32_const,
    I64_const,
    F32_const,
    F64_const,
    Global_get,
    NONE
};

Type Hex_to_type(uint8_t byte);