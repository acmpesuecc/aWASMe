#include<stack>
#include"module/module.hpp"
#include <iostream>
#include <unordered_map>

static const std::unordered_map<IntType, ValueType> intMap = {
    {IntType::i32, ValueType::i32},
    {IntType::i64, ValueType::i64}
};
static const std::unordered_map<FloatType, ValueType> floatMap = {
    {FloatType::f32, ValueType::f32},
    {FloatType::f64, ValueType::f64}
};

struct ValidationData
{
    std::stack<ValueType> ValueStack;
    std::stack<Scope> ScopeStack;
    std::vector<ValueType> parms; //of the current func
    std::vector<ValueType> returns;
    Module::Function *func;

};

bool validate_Function(std::vector<Module::Function> functions,std::vector<Module::Type> types);

bool validate_UINT(ValidationData &data, UnaryInt &instr);
bool validate_UBIT(ValidationData &data, UnaryBitwise &instr);
bool validate_UFLOAT(ValidationData &data, UnaryFloat &instr);

bool validate_Local(ValidationData &data, Local &local);

bool validate_LoadConst(ValidationData &data, LoadConst &instr);

bool validate_Return(ValidationData &data, Return &instr);

bool validate_Scope(ValidationData &data, Scope &scope); //ONLY IF IS VALIDATED

bool validate_End(ValidationData &data, End &end);

bool validate_Arithmetic(std::stack<ValueType> &ValueStack, Instruction &i);

bool validate_ReinterpretBits(ValidationData &data, ReinterpretBits &reinterpretbit);

bool validate_Int(ValidationData &data, IntType num_type, bool fixedResult=false);
bool validate_Float(ValidationData &data, FloatType num_type, bool fixedResult=false);


struct StackValidator
{
    ValidationData &data;
    bool operator()(Local &local){return validate_Local(data,local);}
    bool operator()(LoadConst &instr){return validate_LoadConst(data,instr);}
    bool operator()(Return &instr){return validate_Return(data,instr);}
    bool operator()(Scope &scope){return validate_Scope(data,scope);}
    bool operator()(End &end){return validate_End(data,end);}
    
    bool operator()(IntArithmetic &instr){return validate_Int(data,instr.num_type);}
    bool operator() (BinaryBitwise &instr){return validate_Int(data,instr.num_type);}
    bool operator()(IntCmp &instr){return validate_Int(data,instr.num_type,true);}
    bool operator()(FloatArithmetic &instr){return validate_Float(data,instr.num_type);}
    bool operator()(BinaryFloat &instr){return validate_Float(data,instr.num_type);}
    bool operator()(FloatCmp &instr){return validate_Float(data,instr.num_type,true);}


    bool operator()(UnaryInt &instr){return validate_UINT(data,instr);}
    bool operator()(UnaryBitwise &instr){return validate_UBIT(data,instr);}
    bool operator()(UnaryFloat &instr){return validate_UFLOAT(data,instr);}
    

    bool operator()(ReinterpretBits &reinterpretbit){return validate_ReinterpretBits(data,reinterpretbit);}

    //TO-DO

    bool operator()(Nop &instr)              { return false; }   
    bool operator()(Unreachable &instr)      { return false; }   
    bool operator()(Br &instr)               { return false; }  
    bool operator()(Call &instr)             { return false; }  
    bool operator()(Global &instr)           { return false; }  
    bool operator()(IntConverters &instr)    { return false; }  
    bool operator()(FloatConverters &instr)  { return false; }  
    bool operator()(FloatToIntTrunc &instr)  { return false; }  
    bool operator()(IntToFloat &instr)       { return false; }  

};

