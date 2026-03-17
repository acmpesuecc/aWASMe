#ifndef GLOBAL_HPP 
#define GLOBAL_HPP

#include<engine/value.hpp>

typedef struct {
	Value value;
	bool is_mutable;
} GlobalVar;

#endif
