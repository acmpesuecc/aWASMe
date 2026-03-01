#include"value.hpp"

ValueType to_value_type(Value v) {
	if(std::holds_alternative<int32_t>(v))  return ValueType::i32;
	if(std::holds_alternative<int64_t>(v))  return ValueType::i64;
	if(std::holds_alternative<float>(v))  return ValueType::f32;
	if(std::holds_alternative<double>(v))  return ValueType::f64;
	__builtin_unreachable();

}

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
	if(std::holds_alternative<int32_t>(v))  return std::to_string(std::get<int32_t>(v));
	if(std::holds_alternative<int64_t>(v))  return std::to_string(std::get<int64_t>(v));
	if(std::holds_alternative<float>(v))  return std::to_string(std::get<float>(v));
	if(std::holds_alternative<double>(v))  return std::to_string(std::get<double>(v));
	__builtin_unreachable();
}
