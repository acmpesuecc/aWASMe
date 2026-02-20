#ifndef VALUE_HPP
#define VALUE_HPP

#include<variant>
#include<cstdint>
#include<stdfloat>

typedef std::variant<int32_t,int64_t,float,double> Value;

#endif
