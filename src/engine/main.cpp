/*to execute: 
	make && ./main
  */

#include<iostream>
#include"vm.hpp"
#include"errors.hpp"

int main() {
	VM vm;

	std::vector<Instruction> program = { // proof of concept
		LoadConst{34},
		End{},
		LoadConst{35},
		Arithmetic{.op_kind = Arithmetic::Kind::Add,.num_type = ValueType::i32},
		LoadConst{0}, // Change this from 0 to any non zero number and see the result!
	Scope{.kind = Scope::Kind::If,.info = BlockInfo{.block_start = 4,6,ValueType::i32},.else_info=BlockInfo{6,8,ValueType::i32},8},
			LoadConst{400},
		End{},
			LoadConst{69}, // else block
		End{},
		Cmp{.op_kind = Cmp::Kind::Eq,.num_type = ValueType::i32}
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
