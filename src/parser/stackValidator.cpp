#include "parser/stackValidator.hpp"

void validate_Function(std::vector<Module::Function> functions,std::vector<Module::Type> types)
{
    size_t func_count = functions.size();
    int current_func=0;
    std::cout<<"Number of functions: "<<func_count<<std::endl;
    while(current_func < func_count)
    {
        Module::Function *func = &functions[current_func];
        std::cout<<"Function at index "<<current_func<<" is using signature from index"<<(int)func->typeIndex<<std::endl;
        std::stack<ValueType> ValueStack;
        std::vector<ValueType> parms = types[func->typeIndex].params;
        std::vector<ValueType> returns = types[func->typeIndex].returns;
        std::vector<Instruction> code = func->code;
        std::cout<<"Parm count"<<parms.size()<<"\nLocal coun "<<func->locals.size()<<"\nCOUNT: ";
        for(Instruction i:code)
        {
            if(std::holds_alternative<Local>(i))
            {
                Local *local =&std::get<Local> (i);
                switch (local->kind)
                {
                case Local::Kind::Get:
                    if(local->index<parms.size())
                    {
                        std::cout<<"Local.get at index"<<local->index<<std::endl;
                        ValueStack.push(parms[local->index]);
                    }
                    else if(func->locals.size()>local->index-parms.size())
                    {
                        std::cout<<"Local.get on Local at index"<<local->index-parms.size()<<std::endl;
                        ValueStack.push(func->locals[local->index-parms.size()]);
                    }
                    else 
                    std::cout<<"LOCAL GET FAILED\n";
                    break;
                case Local::Kind::Set:
                {
                    if(ValueStack.empty())
                    {
                        std::cout<<"STACK EMPTY\n";
                        break;
                    }
                    ValueType top = ValueStack.top();
                    if(local->index<parms.size())
                    {
                        std::cout<<"Local.set at index"<<local->index<<std::endl;
                        if(parms[local->index]==top)
                        std::cout<<"local.set Validation passed \n";
                        ValueStack.pop();
                    }
                    else
                    {
                        std::cout<<"Local.set on Local at index"<<local->index-parms.size()<<std::endl;
                        if(func->locals[local->index-parms.size()]==top)
                        std::cout<<"local.set Validation passed \n";
                        ValueStack.pop();
                    }
                    break;
                }
                case Local::Kind::Tee:
                {
                    if(ValueStack.empty())
                    {
                        std::cout<<"STACK EMPTY\n";
                        break;
                    }
                    ValueType top = ValueStack.top();
                    if(local->index<parms.size())
                    {
                        std::cout<<"Local.set at index"<<local->index<<std::endl;
                        if(parms[local->index]==top)
                        std::cout<<"local.tee Validation passed \n";
                    }
                    else
                    {
                        std::cout<<"Local.set on Local at index"<<local->index-parms.size()<<std::endl;
                        if(func->locals[local->index-parms.size()]==top)
                        std::cout<<"local.tee Validation passed \n";
                    }
                    break;
                }
                
                default:
                    break;
                }
            }
            //INT & FLOAT ARITHMETIC && BINARY BITWISE && BINARY FLOAT.
            else if(std::holds_alternative<IntArithmetic>(i)||std::holds_alternative<FloatArithmetic>(i)||std::holds_alternative<BinaryBitwise>(i)||std::holds_alternative<BinaryFloat>(i)||std::holds_alternative<IntCmp>(i)||std::holds_alternative<FloatCmp>(i))
            {
                if(ValueStack.size()<2)
                {
                    std::cout<<"VALIDATION FAILED NOT ENOUGH VARS ON STACK\n";
                    break;
                }
                ValueType top1 = ValueStack.top();
                ValueStack.pop();
                ValueType top2 = ValueStack.top();
                ValueStack.pop();
                if(top1==top2)
                {
                    if(std::holds_alternative<IntArithmetic>(i))
                    {
                        IntArithmetic *instr =&std::get<IntArithmetic>(i);
                        if(instr->num_type==IntType::i32&&top1==ValueType::i32)
                        {
                            std::cout<<"Valid(i32)\n";
                            ValueStack.push(ValueType::i32);
                        }  
                        else if(instr->num_type==IntType::i64&&top1==ValueType::i64)
                        {
                            std::cout<<"Valid(i64)\n";
                            ValueStack.push(ValueType::i64);
                        }
                        else
                        {
                            std::cout << "top1: " << (int)top1 << " top2: " << (int)top2 << " instr num_type: " << (int)instr->num_type << "\n";
                            std::cout<<"Validation failed(intArithmetic)\n";
                        }
                    }
                    else if(std::holds_alternative<FloatArithmetic>(i))
                    {
                        FloatArithmetic *instr=&std::get<FloatArithmetic>(i);
                        if(instr->num_type==FloatType::f32&&top1==ValueType::f32)
                        {
                            std::cout<<"Valid(f32)\n";
                            ValueStack.push(ValueType::f32);
                        }  
                        else if(instr->num_type==FloatType::f64&&top1==ValueType::f64)
                        {
                            std::cout<<"Valid(f64)\n";
                            ValueStack.push(ValueType::f64);
                        }
                        else
                        std::cout<<"Validation failed(FloatArithmetic)\n";     
                    }   
                    else if(std::holds_alternative<BinaryFloat>(i))
                    {
                        BinaryFloat *instr =&std::get<BinaryFloat>(i);
                        if(instr->num_type==FloatType::f32&& top1==ValueType::f32)
                        std::cout<<"Valid(binary_f32)\n";
                        else if(instr->num_type==FloatType::f64&& top1==ValueType::f64)
                        std::cout<<"Valid(binary_f64)\n";

                        ValueStack.push(ValueType::i32);
                    }    
                    else if(std::holds_alternative<BinaryBitwise>(i))
                    {
                        BinaryBitwise *instr =&std::get<BinaryBitwise>(i);
                        //same as IntArthematic (could be made better)
                        if(instr->num_type==IntType::i32&&top1==ValueType::i32)
                        {
                            std::cout<<"Valid(i32)\n";
                            ValueStack.push(ValueType::i32);
                        }  
                        else if(instr->num_type==IntType::i64&&top1==ValueType::i64)
                        {
                            std::cout<<"Valid(i64)\n";
                            ValueStack.push(ValueType::i64);
                        }
                        else
                        {
                            std::cout << "top1: " << (int)top1 << " top2: " << (int)top2 << " instr num_type: " << (int)instr->num_type << "\n";
                            std::cout<<"Validation failed(BINARY BITWISE)\n";
                        }
                    }             
                }
                else
                std::cout<<"Validation failed (Stack ele do not match type)\n";
                
            }
            //INT AND FLOAT UNARY
            else if(std::holds_alternative<UnaryInt>(i)||std::holds_alternative<UnaryFloat>(i))
            {
                if(validate_Unary(ValueStack,i));
            }
            else
            std::cout<<"OTHER INSTR\n";
        }
        current_func++;
        std::cout<<"----------------------------------------------------------------------\n";
    }
}


bool validate_Unary(std::stack<ValueType> &ValueStack, Instruction i)
{
    if(ValueStack.empty())
    {
        std::cout<<"INVALID (stack empty)\n";
        return false;
    }
    ValueType top = ValueStack.top();
    if(std::holds_alternative<UnaryInt>(i))
    {
        UnaryInt *instr =&std::get<UnaryInt>(i);
        if(instr->num_type==IntType::i32&&top==ValueType::i32)
        std::cout<<"Valid(U_i32)\n";
        else if(instr->num_type==IntType::i64&&top==ValueType::i64)
        std::cout<<"Valid(U_i64)\n";
        else
        {
            std::cout<<"INVALID (Unary_I)\n";
            return false;
        }
    }
    else
    {
        UnaryFloat *instr =&std::get<UnaryFloat>(i);
        if(instr->num_type==FloatType::f32&&top==ValueType::f32)
        std::cout<<"Valid(U_f32)\n";
        else if(instr->num_type==FloatType::f64&&top==ValueType::f64)
        std::cout<<"Valid(U_f64)\n";
        else
        {
            std::cout<<"INVALID (Unary_F)\n";
            return false;
        }
    }
    return true;
}
