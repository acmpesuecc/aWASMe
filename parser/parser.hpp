#pragma once
#include<string>
#include<span>
#include "module.hpp"

//Section parsing 
void parse_code_section(std::span<const uint8_t> data, Module& module);

//Helpers
std::vector<uint8_t> Loadfile(std::string Path);
size_t leb128_decode(std::span<const uint8_t> data, size_t size, size_t& offset);

//Data table - mapping binary instructions to their opcodes
