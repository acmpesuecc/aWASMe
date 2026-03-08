/*to execute: 
	make && ./main
  */

#include<iostream>
#include"vm.hpp"
#include"errors.hpp"

int main() {
	VM vm;

	// Calculator program
	// takes in two numbers and another number for the type of operation to perform

	const int32_t to_add = 0;
	const int32_t to_sub = 1;
	const int32_t to_mul = 2;

	int32_t a,b;
	int32_t opt;

	std::cout << "Enter the i32 integer operands: \n";
	std::cin >> a >> b;

	std::cout << "Enter your option\n\t0. To add\n\t1. To subtract\n\t2. To multiply\n\t";
	std::cin >> opt;
	if(!(0<=opt<=to_mul)) {

		std::cout << "Invalid option " << opt << ". Aborting\n";
		return 1;
	}

	std::vector<Instruction> program = { 
		LoadConst{opt},
		LoadConst{to_add},
		Cmp{Cmp::Kind::Eq, ValueType::i32},
		Scope{.kind = Scope::Kind::If,.info = BlockInfo{3,8,ValueType::i32},std::nullopt,std::nullopt},
			LoadConst{b},	
			LoadConst{a},
			Arithmetic{Arithmetic::Kind::Add, ValueType::i32},
			Unreachable{},
		End{},
		LoadConst{opt},
		LoadConst{to_sub},
		Cmp{Cmp::Kind::Eq, ValueType::i32},
		Scope{.kind = Scope::Kind::If,.info = BlockInfo{12,17,ValueType::i32},std::nullopt,std::nullopt},
			LoadConst{b},	
			LoadConst{a},
			Arithmetic{Arithmetic::Kind::Sub, ValueType::i32},
			Unreachable{},
		End{},
		LoadConst{opt},
		LoadConst{to_mul},
		Cmp{Cmp::Kind::Eq, ValueType::i32},
		Scope{.kind = Scope::Kind::If,.info = BlockInfo{21,26,ValueType::i32},std::nullopt,std::nullopt},
			LoadConst{b},	
			LoadConst{a},
			Arithmetic{Arithmetic::Kind::Mul, ValueType::i32},
			Unreachable{},
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
