#ifndef GLOBAL_HPP 
#define GLOBAL_HPP

#include"module/value.hpp"

typedef struct {
	Value value;
	bool is_mutable;
} GlobalVar;

#endif
