#pragma once
#include <optional>
#include "value.hpp"

typedef struct {
	size_t block_start; // index into the start of the block (i.e, the instruction which creates the block and NOT the first instruction of the block)
	size_t block_end; // index into the `end` instruction of the block 

	std::optional<ValueType> return_type;
}BlockInfo;

enum BrAction {
	JumpToEnd,
	JumpToStart
};

struct Block {
	BrAction br_action;
	BlockInfo info;
};

struct Nop {};
struct Unreachable {};


struct LoadConst {
	Value value;
};

struct Arithmetic {
	enum class Kind {
		Add,
		Sub,
		Mul,
	};

	Kind op_kind;
	ValueType num_type;
};

enum IntType {
	i32,
	i64
};

struct UnaryBitwise {
	enum Kind {
		Not
	};
	Kind op_kind;
	// Only integer types can perform these instruction
	IntType num_type;
};

struct BinaryBitwise {
	enum Kind {
		And,
		Or,
		Xor,
		Shl,
		ShrU,
		ShrS
	};

	Kind op_kind;
	// Only integer types can perform these instruction
	IntType num_type;
};

struct Cmp {
	enum class Kind {
		Eq,
		Ne,
		Lt,
		Gt,
		Le,
		Ge
	};

	Kind op_kind;
	ValueType num_type;
};

struct Scope {
	enum Kind {
		Block,
		Loop,
		If,
	};

	Kind kind;
	BlockInfo info;
	std::optional<BlockInfo> else_info; // used in if-else blocks
	std::optional<size_t> if_else_end; // index into the end of the if-else block
};


struct End {};
struct Return {};

struct Call {
	size_t index;	
};

struct Br { size_t index; };


struct Local {
	enum Kind {
		Get,
		Set,
		Tee
	};

	Kind kind;
	size_t index;
};

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

template<class... Ts>
overloads(Ts...) -> overloads<Ts...>;
 
using Instruction = std::variant<
	Nop,
	Unreachable,
	LoadConst,
	Arithmetic,
	Cmp,	
	UnaryBitwise,
	BinaryBitwise,
	Scope,
	End,
	Return,
	Br,
	Call,
	Local
>;

std::string to_string(Instruction i);
