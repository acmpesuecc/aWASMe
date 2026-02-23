/*To execute: 
	g++ -I./include src/engine/main.cpp src/engine/vm.cpp -o main
	./main
*/

#include<iostream>
#include"engine/vm.hpp"

int main() {
	VM vm;

    std::vector<Instruction> program = { 		// proof of concept
	{InstrKind::I32_CONST, {.value = 34}},
	{InstrKind::I32_CONST, {.value = 35}},
	{InstrKind::I32_ADD},
	{InstrKind::I32_CONST, {.value = 1}}, // Change this from 0 to any non zero number and see the result!
	{InstrKind::IF, { .if_ = {{4,6}} }},
		{InstrKind::I32_CONST, {.value = 420}},
	{InstrKind::END},
    };

    vm.load(program);
    vm.run();

    auto result = vm.pop();

    if (result.has_value()) {
        std::cout << "Result: " << std::get<int32_t>(result.value()) << "\n";		//takes only int_32 type
    } else {
        std::cout << "Underflow: No result on stack\n";
    }

    return 0;
}
