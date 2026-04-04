#include<stack>
#include"module/module.hpp"
#include <iostream>
#include <unordered_map>

void validate_Function(std::vector<Module::Function> functions,std::vector<Module::Type> types);

bool validate_Unary(std::stack<ValueType> &ValueStack,Instruction &i);

static const std::unordered_map<IntType, ValueType> intMap = {
    {IntType::i32, ValueType::i32},
    {IntType::i64, ValueType::i64}
};
static const std::unordered_map<FloatType, ValueType> floatMap = {
    {FloatType::f32, ValueType::f32},
    {FloatType::f64, ValueType::f64}
};