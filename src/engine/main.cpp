/*to execute: 
	make && ./main
  */

#include<iostream>
#include"engine/vm.hpp"
#include"engine/errors.hpp"

int main() {
	VM vm;

	// Calculator program
	// takes in two numbers and another number for the type of operation to perform

	const int32_t to_add = 0;
	const int32_t to_sub = 1;
	const int32_t to_mul = 2;

	float a,b;
	int32_t opt;

	std::cout << "Enter the f32 integer operands: \n";
	std::cin >> a >> b;

	std::cout << "Enter your option\n\t0. To add\n\t1. To subtract\n\t2. To multiply\n\t";
	std::cin >> opt;
	if(opt < 0 || opt > to_mul){
		std::cout << "Invalid option " << opt << ". Aborting\n";
		return 1;
	}


	std::vector<Instruction> program = { 
		LoadConst{opt},
		LoadConst{to_add},
		IntCmp{IntCmp::Kind::Eq, IntType::i32},
		Scope{Scope::Kind::If,BlockInfo{3,8,ValueType::f32},std::nullopt,std::nullopt},
			LoadConst{b},	
			LoadConst{a},
			FloatArithmetic{FloatArithmetic::Kind::Add, FloatType::f32},
			Unreachable{},
		End{},
		LoadConst{opt},
		LoadConst{to_sub},
		IntCmp{IntCmp::Kind::Eq, IntType::i32},
		Scope{Scope::Kind::If, BlockInfo{12,17,ValueType::f32},std::nullopt,std::nullopt},
			LoadConst{b},	
			LoadConst{a},
			FloatArithmetic{FloatArithmetic::Kind::Sub, FloatType::f32},
			Unreachable{},
		End{},
		LoadConst{opt},
		LoadConst{to_mul},
		IntCmp{IntCmp::Kind::Eq, IntType::i32},
		Scope{Scope::Kind::If, BlockInfo{21,26,ValueType::f32},std::nullopt,std::nullopt},
			LoadConst{b},	
			LoadConst{a},
			FloatArithmetic{FloatArithmetic::Kind::Mul, FloatType::f32},
			Unreachable{},
		End{}
		};


	vm.load(program);
	vm.run();

	auto result = vm.pop();

	if (result.has_value()) {
		std::cout << "Result: " << std::get<float>(result.value()) << "\n";		//takes only int_32 type
	} else {
		std::cout << "Underflow: No result on stack\n";
	}

	return 0;
}
