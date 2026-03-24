#ifndef INSTRUCTION_HPP 
#define INSTRUCTION_HPP

#include <optional>

#include "module/value.hpp"

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

enum FloatType {
	f32,
	f64
};

enum IntType {
	i32,
	i64
};

struct IntArithmetic {
	enum class Kind {
		Add,
		Sub,
		Mul,
		DivS,
		DivU,
		RemS,
		RemU
	};

	Kind op_kind;
	IntType num_type;
};

struct FloatArithmetic {
	enum class Kind {
		Add,
		Sub,
		Mul,
		Div,
	};

	Kind op_kind;
	FloatType num_type;
};


struct UnaryInt {
	enum class Kind {
		Clz,
		Ctz,
		Popcnt
	};

	Kind op_kind;
	IntType num_type;
};

struct UnaryFloat {
	enum class Kind {
		Abs,
		Neg,
		Ceil,
		Floor,
		Trunc,
		Nearest,
		Sqrt
	};

	Kind op_kind;
	FloatType num_type;
};

struct BinaryFloat {
	enum class Kind {
		Min,
		Max,
		CopySign
			
	};

	Kind op_kind;
	FloatType num_type;
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

struct IntCmp {
	enum class Kind {
		Eq,
		Ne,

		LtU,
		GtU,
		LeU,
		GeU,

		LtS,
		GtS,
		LeS,
		GeS,
	};
	Kind op_kind;
	IntType num_type;
};

struct FloatCmp {
	enum class Kind {
		Eq,
		Ne,
		Lt,
		Gt,
		Le,
		Ge,
	};

	Kind op_kind;
	FloatType num_type;
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

struct Br { 
	size_t index; 
	/// If this is false, then it behaves like a br_if
	bool is_unconditional;
};

struct Local {
	enum Kind {
		Get,
		Set,
		Tee
	};

	Kind kind;
	size_t index;
};

struct Global {
	enum Kind {
		Get,
		Set,
	};

	Kind kind;
	size_t index;
};

/// i32 and i64 interconverters 
struct IntConverters {
	enum {
		Wrap,
		ExtendU,
		ExtendS
	} kind;
	
};

/// f32 and f64 interconverters 
struct FloatConverters {
	enum {
		Promote,
		Demote,
	} kind;
};

struct FloatToIntTrunc {
	IntType to;
	FloatType from;
	bool is_signed;
};

struct IntToFloat {
	FloatType to;
	IntType from;
	bool is_signed;
};

struct ReinterpretBits {
	// We only take one value type, because if we have it, we automatically know (or rather, its automatically set) what the type to convert to will be
	//
	// Example: reinterpreting i32 would be f32, and reinterpreting f64 would be i64
	ValueType from;
};

template<class... Ts>
struct overloads : Ts... { using Ts::operator()...; };

template<class... Ts>
overloads(Ts...) -> overloads<Ts...>;
 
using Instruction = std::variant<
	Nop,
	Unreachable,
	LoadConst,
	IntArithmetic,
	FloatArithmetic,
	IntCmp,	
	FloatCmp,	
	UnaryInt,
	UnaryFloat,
	BinaryFloat,
	UnaryBitwise,
	BinaryBitwise,
	Scope,
	End,
	Return,
	Br,
	Call,
	Local,
	Global,
	IntConverters,
	FloatConverters,
	FloatToIntTrunc,
	IntToFloat,
	ReinterpretBits
>;

std::string to_string(Instruction i);

#endif
