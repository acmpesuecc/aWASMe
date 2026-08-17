#include "parser/stackValidator.hpp"

bool validate_Function(std::vector<Module::Function> functions,std::vector<Module::Type> types)
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
        ValidationData data={ValueStack,ScopeStack,parms,returns,func};

        for(Instruction &i:code)
        {
            if(!std::visit(StackValidator{data},i))
            {
                return false;
            }
        }
        current_func++;
    }
    return true;
}

bool validate_Local(ValidationData &data,Local &local)
{
    switch (local.kind)
    {
    case Local::Kind::Get:
    {
        if(local.index<data.parms.size())
        {
            std::cout<<"Local.get at index"<<local.index<<std::endl;
            data.ValueStack.push(data.parms[local.index]);
        }
        else if(data.func->locals.size()>local.index-data.parms.size())
        {
            std::cout<<"Local.get on Local at index"<<local.index-data.parms.size()<<std::endl;
            data.ValueStack.push(data.func->locals[local.index-data.parms.size()]);
        }
        else 
        {
            std::cout<<"LOCAL GET FAILED\n";
            return false;
        }
        return true;
    }
    case Local::Kind::Set:
    {
        if(data.ValueStack.empty())
        {
            std::cout<<"STACK EMPTY\n";
            return false;
        }
        ValueType top = data.ValueStack.top();
        if(local.index<data.parms.size())
        {
            std::cout<<"Local.set at index"<<local.index<<std::endl;
            if(data.parms[local.index]==top)
            std::cout<<"local.set Validation passed \n";
            data.ValueStack.pop();
        }
        else if(data.func->locals.size()>local.index-data.parms.size())
        {
            std::cout<<"Local.set on Local at index"<<local.index-data.parms.size()<<std::endl;
            if(data.func->locals[local.index-data.parms.size()]==top)
            std::cout<<"local.set Validation passed \n";
            data.ValueStack.pop();
        }
        else
        {
            std::cout<<"LOCAL SET FAILED\n";
            return false;
        }
        return true;
    }
    case Local::Kind::Tee:
    {
        if(data.ValueStack.empty())
        {
            std::cout<<"STACK EMPTY\n";
            return false;
        }
        ValueType top = data.ValueStack.top();
        if(local.index<data.parms.size())
        {
            std::cout<<"Local.set at index"<<local.index<<std::endl;
            if(data.parms[local.index]==top)
            std::cout<<"local.tee Validation passed \n";
        }
        else if(data.func->locals.size()>local.index-data.parms.size())
        {
            std::cout<<"Local.set on Local at index"<<local.index-data.parms.size()<<std::endl;
            if(data.func->locals[local.index-data.parms.size()]==top)
            std::cout<<"local.tee Validation passed \n";
        }
        else
        {
            std::cout<<"LOCAL TEE FAILED\n";
            return false;    
        }
        return true;
    }
    
    default:
        {
            return false;
        }
    }
    return false;
}

bool validate_LoadConst(ValidationData &data, LoadConst &instr)
{
    //int32_t,int64_t,float,double
    if(std::holds_alternative<int32_t>(instr.value))
        data.ValueStack.push(ValueType::i32);
    else if(std::holds_alternative<int64_t>(instr.value))
        data.ValueStack.push(ValueType::i64);
    else if(std::holds_alternative<float>(instr.value))
        data.ValueStack.push(ValueType::f32);
    else if(std::holds_alternative<double>(instr.value))
        data.ValueStack.push(ValueType::f64);
    else
        return false;
    
    std::cout<<"Pushed Const\n";
    return true;

}


bool validate_Return(ValidationData &data, Return &instr)
{
    if(data.returns.empty())
    {
        std::cout<<"RETURNS VOID\n";
        return false;
    }
    else
    {
        if(data.ValueStack.empty())
        {
            std::cout<<"Invalid stack empty\n";
            return false;
        }
        else if(data.ValueStack.top()==data.returns[0])
        {
            std::cout<<"Return Valid\n";
            return true;
        }
        else
        {
            std::cout<<"Return Invalid\n";
            return false;
        }
    }
    return false;

}

bool validate_Scope(ValidationData &data, Scope &scope)
{
    if(scope.kind==Scope::Kind::If)
    {
        if(data.ValueStack.empty()||data.ValueStack.top()!=ValueType::i32)
        {
            std::cout<<"Scope Invalid(if)\n";
            return false;
        }
        data.ValueStack.pop();
    }
    data.ScopeStack.push(scope);
    std::cout<<"Scope Pushed\n";
    return true;
}

bool validate_End(ValidationData &data, End &end)
{

    if(data.ScopeStack.empty())
    {
        std::cout<<"Function end\n";
        return true;
    }
    Scope scopeTop= data.ScopeStack.top();
    if(scopeTop.info.return_type.has_value())
    {
        if(data.ValueStack.empty() || data.ValueStack.top() != scopeTop.info.return_type.value())
        {
            std::cout<<"INVALID: block return type mismatch\n";
            return false;
        }
        else
        {
            std::cout<<"block end valid\n";
            data.ValueStack.pop();
        }
    }
    if(!(scopeTop.else_info.has_value() && scopeTop.kind == Scope::Kind::If))
    {
        data.ScopeStack.pop();
        // push result type back for parent scope to consume
        if(scopeTop.info.return_type.has_value())
        data.ValueStack.push(scopeTop.info.return_type.value());
    }
    else
    {
        data.ScopeStack.top().else_info = std::nullopt; // clear so next End pops
    }   
    return true; 
}

bool validate_ReinterpretBits(ValidationData &data, ReinterpretBits &reinterpretbit)
{
    if(data.ValueStack.empty()||reinterpretbit.from !=data.ValueStack.top())
    {
        std::cout<<"Ivalid stack empty or type mismatch(ReinterpretBits)"<<std::endl;
        return false;
    }
    else
    {
        data.ValueStack.pop();
        switch(reinterpretbit.from)
    {
        case ValueType::i32:
        {
            data.ValueStack.push(ValueType::f32);
            break;
        }
        case ValueType::i64:
        {
            data.ValueStack.push(ValueType::f64);
            break;
        }
        case ValueType::f32:
        {
            data.ValueStack.push(ValueType::i32);
            break;
        }
        case ValueType::f64:
        {
            data.ValueStack.push(ValueType::i64);
            break;
        }
        default:
        {
            std::cout<<"INVALID ValueType"<<std::endl;
            return false;
        }
    }

    }
    return true;

}

bool validate_UINT(ValidationData &data, UnaryInt &instr)
{
    if(data.ValueStack.empty())
    {
        std::cout<<"INVALID (stack empty)\n";
        return false;
    }
    ValueType top = data.ValueStack.top();        
    if(instr.num_type==IntType::i32&&top==ValueType::i32)
    std::cout<<"Valid(U_i32)\n";
    else if(instr.num_type==IntType::i64&&top==ValueType::i64)
    std::cout<<"Valid(U_i64)\n";
    else
    {
        std::cout<<"INVALID (Unary_I)\n";
        return false;
    }
    return true;

}

bool validate_UBIT(ValidationData &data, UnaryBitwise &instr)
{
    if(data.ValueStack.empty())
    {
        std::cout<<"INVALID (stack empty)\n";
        return false;
    }
    ValueType top =data.ValueStack.top();
    if(instr.num_type==IntType::i32&&top==ValueType::i32)
    std::cout<<"Valid(U_i32)\n";
    else if(instr.num_type==IntType::i64&&top==ValueType::i64)
    std::cout<<"Valid(U_i64)\n";
    else
    {
        std::cout<<"INVALID (Unary_B)\n";
        return false;
    }
    return true;
}

bool validate_UFLOAT(ValidationData &data, UnaryFloat &instr)
{
    if(data.ValueStack.empty())
    {
        std::cout<<"INVALID (stack empty)\n";
        return false;
    }
    ValueType top =data.ValueStack.top();
    if(instr.num_type==FloatType::f32&&top==ValueType::f32)
    std::cout<<"Valid(U_f32)\n";
    else if(instr.num_type==FloatType::f64&&top==ValueType::f64)
    std::cout<<"Valid(U_f64)\n";
    else
    {
        std::cout<<"INVALID (Unary_F)\n";
        return false;
    }

    return true;
}

bool validate_Int(ValidationData& data, IntType num_type, bool fixedResult)
{
    if(data.ValueStack.size()<2)
    {
        std::cout<<"VALIDATION FAILED NOT ENOUGH VARS ON STACK\n";
        return false;
    }
    ValueType top1 = data.ValueStack.top();
    data.ValueStack.pop();
    ValueType top2 = data.ValueStack.top();
    data.ValueStack.pop();
    if(top1==top2)
    {
        auto it = intMap.find(num_type);
        if (it != intMap.end() && top1 == it->second)
        {
            data.ValueStack.push(fixedResult ? ValueType::i32 : it->second);
            std::cout << "Valid\n";
            return true;
        } 
        else 
        {
            std::cout << "VALIDATION FAILED: int type mismatch\n";
            return false;
        }
    }
    return false;
}
bool validate_Float(ValidationData& data, FloatType num_type, bool fixedResult)
{
    if(data.ValueStack.size()<2)
    {
        std::cout<<"VALIDATION FAILED NOT ENOUGH VARS ON STACK\n";
        return false;
    }
    ValueType top1 = data.ValueStack.top();
    data.ValueStack.pop();
    ValueType top2 = data.ValueStack.top();
    data.ValueStack.pop();
    if(top1==top2)
    {
        auto it = floatMap.find(num_type);
        if (it != floatMap.end() && top1 == it->second) 
        {
            data.ValueStack.push(fixedResult ? ValueType::i32 : it->second);
            std::cout << "Valid\n";
            return true;
        }
        else
        {
            std::cout << "VALIDATION FAILED: float type mismatch\n";
            return false;
        }
    }
    return false;
}


