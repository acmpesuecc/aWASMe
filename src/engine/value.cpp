#include "module/value.hpp"
#include<emscripten/bind.h>

template<class... Ts>
struct overloads : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloads(Ts...) -> overloads<Ts...>;

ValueType to_value_type(Value v) {
	if(std::holds_alternative<int32_t>(v))  return ValueType::i32;
	if(std::holds_alternative<int64_t>(v))  return ValueType::i64;
	if(std::holds_alternative<float>(v))  return ValueType::f32;
	if(std::holds_alternative<double>(v))  return ValueType::f64;
	__builtin_unreachable();
}

Value zero_from_value_type(ValueType v) {
	switch(v) {
		case ValueType::i32: return (int32_t)0;
		case ValueType::i64: return (int64_t)0;
		case ValueType::f32: return (float)0.0;
		case ValueType::f64: return (double)0.0;
	}
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

emscripten::val to_js_val(Value val) {
	return std::visit(overloads{
			[](int32_t i) { return emscripten::val(i); },
			[](int64_t i) { return emscripten::val::global("BigInt")(i); },
			[](float i) { return emscripten::val(i); },
			[](double i) { return emscripten::val(i); }
			},val);
}

emscripten::val to_js_value_vector(std::vector<Value> inp) {
	emscripten::val out = emscripten::val::array();
	size_t i = 0;
	for(auto it = inp.begin(); it != inp.end(); it++,i++) {
		out.set(i,to_js_val(*it));
	}
	return out;
}


Value from_js_value(emscripten::val v,ValueType ty) {
	switch(ty) {
		case ValueType::i32: return v.as<int32_t>();
		case ValueType::i64: return v.as<int64_t>();
		case ValueType::f32: return v.as<float>();
		case ValueType::f64: return v.as<double>();

	}
}
