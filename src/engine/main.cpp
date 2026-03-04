/*to execute: 
	make && ./main
  */

#include<iostream>
#include"vm.hpp"
#include"errors.hpp"

int main() {
	VM vm;

	FunctionInfo fi = {.block_info = BlockInfo{2,18,ValueType::i32},{ValueType::i32},{}};
	size_t n = vm.register_function(fi);

	int32_t start_num = 0
	int32_t end_num = 420;
	int32_t step = 1;

	// proof of concept
	// this program start from start_num, add step and repeat until you are greater than or equal to end_num
	std::vector<Instruction> program = { 
		LoadConst{start_num},
		Call{n},
		Unreachable{},
		Scope({Scope::Kind::Loop,BlockInfo{3,15,ValueType::i32},std::nullopt,std::nullopt}),
			Local{.kind=Local::Kind::Get,.index=0},
			LoadConst{step},
			Arithmetic{.op_kind=Arithmetic::Kind::Add,.num_type=ValueType::i32},
			Local{.kind=Local::Kind::Set,.index=0},
			LoadConst{end_num},
			Local{.kind=Local::Kind::Get,.index=0},
			Cmp{Cmp::Kind::Lt,ValueType::i32},
			Scope({Scope::Kind::If,BlockInfo{11,13,{}},{},13}),
				Br{1},
			End{},
			Local{.kind=Local::Kind::Get,.index=0},
		End{},
		Local{.kind=Local::Kind::Get,.index=0},
		Return{},
		End{}
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
