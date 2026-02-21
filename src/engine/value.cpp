#include"value.hpp"

std::string to_string(ValueType v) {
	switch(v) {
		case ValueType::i32: return "i32";
		case ValueType::i64: return "i64";
		case ValueType::f32: return "f32";
		case ValueType::f64: return "f64";
	}
	__builtin_unreachable();
}

std::string to_string(Value v) {
	if(std::holds_alternative<int32_t>(v))  return to_string(ValueType::i32);
	if(std::holds_alternative<int64_t>(v))  return to_string(ValueType::i64);
	if(std::holds_alternative<float>(v))  return to_string(ValueType::f32);
	if(std::holds_alternative<double>(v))  return to_string(ValueType::f64);
	__builtin_unreachable();
}

