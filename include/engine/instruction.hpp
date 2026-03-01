#ifndef INSTRUCTION_HPP 
#define INSTRUCTION_HPP

#include <optional>
#include <value.hpp>

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

struct Bitwise {
	enum Kind {
		And,
		Or,
		Not,
		Xor,
		Shl,
		ShrU,
		ShrS
	};

	/// The only valid value types that can perform this instruction
	enum VType {
		i32,
		i64

	};

	Kind op_kind;
	VType num_type;
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
	Bitwise,
	Scope,
	End,
	Return,
	Br,
	Call
>;

std::string to_string(Instruction i);

#endif
