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

	ImportedFunction inf = {.index = 0};
	Function f = {.args = {ValueType::i32}, .kind = inf};
	size_t log_int = vm.register_function(f);

	std::vector<Instruction> program = { 
		LoadConst{(int32_t)67},
		Call{log_int}
	};


	vm.load(program);
	vm.run();
	/*
	auto result = vm.pop();

	if (result.has_value()) {
		std::cout << "Result: " << std::get<int32_t>(result.value()) << "\n";		//takes only int_32 type
	} else {
		std::cout << "Underflow: No result on stack\n";
	}
	*/
}

}

EMSCRIPTEN_KEEPALIVE
int main() {
	
	/*
	   auto result = vm.pop();

	   if (result.has_value()) {
	   std::cout << "Result: " << std::get<float>(result.value()) << "\n";		//takes only int_32 type
	   } else {
	   std::cout << "Underflow: No result on stack\n";
	   }

*/
	return 0;
}
