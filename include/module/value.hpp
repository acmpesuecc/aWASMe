#pragma once
#include<variant>
#include<cstdint>
#include<string>
#include<vector>
#include<emscripten/val.h>

typedef std::variant<int32_t,int64_t,float,double> Value;
enum class ValueType {
	i32	= 0x7F,
	i64 	= 0x7E,
	f32 	= 0x7D,
	f64 	= 0x7C,
};

ValueType to_value_type(Value);
Value zero_from_value_type(ValueType);
std::string to_string(ValueType v);
std::string to_string(Value v);

emscripten::val to_js_val(Value);
emscripten::val to_js_value_vector(std::vector<Value> inp);
