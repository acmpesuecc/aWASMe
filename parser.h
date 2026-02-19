#pragma once
#include<iostream>
#include<fstream>
#include<vector>
#include<string>

struct Function
{
    uint8_t funcID;
    std::vector<uint8_t> funcCode;
};

class Module
{
    public:
     std::size_t size;
     std::vector<uint8_t> Loadfile(std::string Path);
     size_t leb128(std::vector<uint8_t>& data, size_t size,size_t &offset);
    std::vector<Function> functions;         
    //std::vector<Memory> memories;         
    //std::vector<Export> exports;          
    //std::vector<Global> globals;       
    uint32_t start_function;           

};

std::vector<uint8_t> Module::Loadfile(std::string Path)
{
    std::ifstream file(Path, std::ios::binary);
    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), size);
    

    if (data[0] == 0x00 && data[1] == 0x61 && data[2] == 0x73 && data[3] == 0x6D)
    {
        std::cout << "WASM Recived , ready to Parse, file size is" << size << std::endl;
    }
    else
        std::cout << "INVALID FILE";
    std::cout << (int)data[8] << std::endl;
    return data;
}


size_t Module::leb128(std::vector<uint8_t>& data, size_t size,size_t &offset)
{

    size_t secSize=0;
    uint8_t Id = data[offset++]; //SECTION ID
    int shift=0;
    while(offset<size)
    {
        if(data[offset]&0x80)
        {
            secSize|=(data[offset]&0x7f)<<shift;
            shift+=7;
        }
        else{
            secSize|=(data[offset]&0x7f)<<shift;
            return secSize;
        }
        offset++;
    }
    return secSize;
}


