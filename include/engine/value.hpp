#ifndef VALUE_HPP
#define VALUE_HPP

#include<variant>
#include<cstdint>
#include<stdfloat>

#include<string>

typedef std::variant<int32_t,int64_t,float,double> Value;
enum class ValueType {
	i32,
	i64,
	f32,
	f64,
};

std::string to_string(ValueType v);
std::string to_string(Value v);

#endif
