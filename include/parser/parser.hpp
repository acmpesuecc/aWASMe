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
void parse_table_section(std::span<const uint8_t>data,Module& module);
void parse_element_section(std::span<const uint8_t>data,Module& module);
void parse_data_section(std::span<const uint8_t>data,Module& module);
void parse_export_section(std::span<const uint8_t>data ,Module& module);
//Helpers
std::vector<uint8_t> Loadfile(std::string Path);
size_t leb128_decode(std::span<const uint8_t> data, size_t size, size_t& offset);
//Helpers for ELEMENT SECTION
void parse_function(std::span<const uint8_t>data,size_t eleCount,size_t &offset,size_t secSize,Module& module);
void parse_expression(std::span<const uint8_t>data,size_t eleCount,size_t &offset,size_t secSize,Module& module);
//Helpers for IMPORT AND EXPORT 
std::string read_string(std::span<const uint8_t>data,size_t &offset);
//Data table - mapping binary instructions to their opcodes
//mapping for TYPE SECTION 
enum class Type {
    I32       =0x7F,
    I64       =0x7E,
    F32       =0x7D,
    F64       =0x7C,
    I32_const =0x41,
    I64_const =0x42,
    F32_const =0x43,
    F64_const =0x44,
    Global_get =0x23,
    NONE
};
enum class Flag : uint8_t
{
    ActiveImplicit          = 0x00,  // active, table 0, func indices
    Passive                 = 0x01,  // passive, func indices
    ActiveExplicit          = 0x02,  // active, explicit table, func indices
    Declarative             = 0x03,  // declarative, func indices
    ActiveImplicitExpr      = 0x04,  // active, table 0, expressions
    PassiveExpr             = 0x05,  // passive, expressions
    ActiveExplicitExpr      = 0x06,  // active, explicit table, expressions
    DeclarativeExpr         = 0x07,  // declarative, expressions
};
enum class Tag : uint8_t
{
       Function = 0x00,
       Table =0x01,
       Memory =0x02,
       Global=0x03, 
};

