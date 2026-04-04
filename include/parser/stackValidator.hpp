#include<stack>
#include"module/module.hpp"
#include <iostream>
void validate_Function(std::vector<Module::Function> functions,std::vector<Module::Type> types);

bool validate_Unary(std::stack<ValueType> &ValueStack,Instruction i);