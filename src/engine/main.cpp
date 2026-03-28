/*to execute: 
	make engine && ./engine.exe
  */

#include<iostream>

#include "engine/vm.hpp"
#include "engine/errors.hpp"

int main() {
	VM vm;

	// Simple memory demo: store two f32 operands in linear memory,
	// load them back, add them, and leave the sum on the stack.

	float a, b;
	std::cout << "Enter two f32 operands: \n";
	std::cin >> a >> b;

	std::vector<Instruction> program = {
		LoadConst{int32_t(0)},
		LoadConst{a},
		MemoryStore{MemoryStore::Kind::F32Store, 0},

		LoadConst{int32_t(4)},
		LoadConst{b},
		MemoryStore{MemoryStore::Kind::F32Store, 0},

		LoadConst{int32_t(0)},
		MemoryLoad{MemoryLoad::Kind::F32Load, 0},
		LoadConst{int32_t(4)},
		MemoryLoad{MemoryLoad::Kind::F32Load, 0},
		FloatArithmetic{FloatArithmetic::Kind::Add, FloatType::f32},
	};

	vm.load(program);
	vm.run();

	auto result = vm.pop();

	if (result.has_value()) {
		std::cout << "Sum: " << std::get<float>(result.value()) << "\n";
	} else {
		std::cout << "Underflow: No result on stack\n";
	}

	return 0;
}
