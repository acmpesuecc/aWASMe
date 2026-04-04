/*to execute: 
  make engine && ./engine.exe
  */

#include<iostream>
#include<emscripten.h>

#include "engine/vm.hpp"
#include "engine/errors.hpp"

extern "C" {
		EMSCRIPTEN_KEEPALIVE
		void do_stuff() {
			VM vm;

			ImportedFunction inf = {.index = 0, .return_type = ValueType::i32};
			Function f = {.args = {ValueType::i32}, .kind = inf};
			size_t log_int = vm.register_function(f);

			std::vector<Instruction> program = { 
				LoadConst{(int32_t)67},
				Call{log_int}
			};


			vm.load(program);
			vm.run();

			auto result = vm.pop();

			if (result.has_value()) {
				std::cout << "Result: " << std::get<int32_t>(result.value()) << "\n";
			} else {
				std::cout << "Underflow: No result on stack\n";
			}

		}

}


EMSCRIPTEN_KEEPALIVE
int main() {
	VM vm;

	// Simple memory demo: store two f32 operands in linear memory,
	// load them back, add them, and leave the sum on the stack.

	float a, b;
	std::cout << "Enter two f32 operands: \n";
	// NOTE: ive not found a way to make this compatible with Node js, but it works on the browser if you compile it to
	// TODO: find a way to make this compatible with nodejs
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

