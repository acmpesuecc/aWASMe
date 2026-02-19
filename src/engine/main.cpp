/*to execute: 
	g++ -I./include/engine src/engine/main.cpp src/engine/vm.cpp -o main
	./main
*/

#include<iostream>
#include"vm.hpp"

int main() {
	VM vm;

    std::vector<Instruction> program = {
        {InstrKind::I32_CONST, 10},		//proof of concept
        {InstrKind::I32_CONST, 20},
        {InstrKind::I32_ADD},
        {InstrKind::END}
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
