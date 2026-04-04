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
        std::stack<Scope> ScopeStack;//for blocks
        std::vector<ValueType> parms = types[func->typeIndex].params;
        std::vector<ValueType> returns = types[func->typeIndex].returns;
        std::vector<Instruction> code = func->code;
        std::cout<<"Parm count"<<parms.size()<<"\nLocal coun "<<func->locals.size()<<"\nCOUNT: ";
        for(Instruction &i:code)
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
                    else if(func->locals.size()>local->index-parms.size())
                    {
                        std::cout<<"Local.set on Local at index"<<local->index-parms.size()<<std::endl;
                        if(func->locals[local->index-parms.size()]==top)
                        std::cout<<"local.set Validation passed \n";
                        ValueStack.pop();
                    }
                    else
                    std::cout<<"LOCAL SET FAILED\n";
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
                    else if(func->locals.size()>local->index-parms.size())
                    {
                        std::cout<<"Local.set on Local at index"<<local->index-parms.size()<<std::endl;
                        if(func->locals[local->index-parms.size()]==top)
                        std::cout<<"local.tee Validation passed \n";
                    }
                    else
                    std::cout<<"LOCAL TEE FAILED\n";
                    break;
                }
                
                default:
                    break;
                }
            }
            else if(std::holds_alternative<LoadConst>(i))
            {
                //int32_t,int64_t,float,double
                LoadConst *instr = &std::get<LoadConst>(i);
                if(std::holds_alternative<int32_t>(instr->value))
                    ValueStack.push(ValueType::i32);
                else if(std::holds_alternative<int64_t>(instr->value))
                    ValueStack.push(ValueType::i64);
                else if(std::holds_alternative<float>(instr->value))
                    ValueStack.push(ValueType::f32);
                else if(std::holds_alternative<double>(instr->value))
                    ValueStack.push(ValueType::f64);
                
                std::cout<<"Pushed Const\n";
            }
            else if(std::holds_alternative<Return>(i))
            {
                if(returns.empty())
                {
                    std::cout<<"RETURNS VOID\n";
                }
                else
                {
                    if(ValueStack.empty())
                    std::cout<<"Invalid stack empty\n";
                    else if(ValueStack.top()==returns[0])
                    {
                        std::cout<<"Return Valid\n";
                    }
                    else
                    std::cout<<"Return Invalid\n";
                }
            }
            else if(std::holds_alternative<Scope>(i))
            {
                Scope *scope=&std::get<Scope>(i);
                if(scope->kind==Scope::Kind::If)
                {
                    if(ValueStack.empty()||ValueStack.top()!=ValueType::i32)
                    {
                        std::cout<<"Scope INValid(if)\n";
                        break;
                    }
                    ValueStack.pop();
                }
                ScopeStack.push(*scope);
                std::cout<<"Scope Pushed\n";
            }
            else if(std::holds_alternative<End>(i))
            {
                if(ScopeStack.empty())
                {
                    std::cout<<"Function end\n";
                    break;
                }
                Scope scopeTop= ScopeStack.top();
                if(scopeTop.info.return_type.has_value())
                {
                    if(ValueStack.empty() || ValueStack.top() != scopeTop.info.return_type.value())
                    {
                        std::cout<<"INVALID: block return type mismatch\n";
                    }
                    else
                    {
                        std::cout<<"block end valid\n";
                        ValueStack.pop();
                    }
                }
                if(!(scopeTop.else_info.has_value() && scopeTop.kind == Scope::Kind::If))
                {
                    ScopeStack.pop();
                    // push result type back for parent scope to consume
                    if(scopeTop.info.return_type.has_value())
                    ValueStack.push(scopeTop.info.return_type.value());
                }
                else
                {
                    ScopeStack.top().else_info = std::nullopt; // clear so next End pops
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

                    auto validateInt = [&](IntType num_type, bool fixedResult = false) 
                    {
                        auto it = intMap.find(num_type);
                        if (it != intMap.end() && top1 == it->second)
                        {
                            ValueStack.push(fixedResult ? ValueType::i32 : it->second);
                            std::cout << "Valid\n";
                        } 
                        else 
                        std::cout << "VALIDATION FAILED: int type mismatch\n";
                    };

                    auto validateFloat = [&](FloatType num_type, bool fixedResult = false)
                    {
                        auto it = floatMap.find(num_type);
                        if (it != floatMap.end() && top1 == it->second) 
                        {
                            ValueStack.push(fixedResult ? ValueType::i32 : it->second);
                            std::cout << "Valid\n";
                        }
                        else
                        std::cout << "VALIDATION FAILED: float type mismatch\n";
                    };

                    if (std::holds_alternative<IntArithmetic>(i))
                        validateInt(std::get<IntArithmetic>(i).num_type);
                        
                    else if (std::holds_alternative<BinaryBitwise>(i))
                        validateInt(std::get<BinaryBitwise>(i).num_type);
                    
                    else if (std::holds_alternative<IntCmp>(i))
                        validateInt(std::get<IntCmp>(i).num_type, true);
                
                    else if (std::holds_alternative<FloatArithmetic>(i))
                        validateFloat(std::get<FloatArithmetic>(i).num_type);
            
                    else if (std::holds_alternative<BinaryFloat>(i))
                        validateFloat(std::get<BinaryFloat>(i).num_type, true);
            
                    else if (std::holds_alternative<FloatCmp>(i))
                         validateFloat(std::get<FloatCmp>(i).num_type, true);
                }
                ///
                else
                std::cout<<"Elements do not match\n";
                                      
                
            }
            //INT AND FLOAT UNARY
            else if(std::holds_alternative<UnaryInt>(i)||std::holds_alternative<UnaryFloat>(i))
            {
                if(validate_Unary(ValueStack,i));
            }
            else
            {
                std::cout<<"OTHER INSTR\n";
            }
            
        }
        current_func++;
        std::cout<<"----------------------------------------------------------------------\n";
    }
}


bool validate_Unary(std::stack<ValueType> &ValueStack, Instruction &i)
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
