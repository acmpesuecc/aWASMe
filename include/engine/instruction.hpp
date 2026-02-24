#ifndef INSTRUCTION_HPP 
#define INSTRUCTION_HPP

#include <optional>
#include <value.hpp>

enum class InstrKind {
    UNREACHABLE,
    NOP,

    I32_CONST,
    I32_ADD,
    I32_SUB,
    I32_MUL,

    BLOCK,
    LOOP,
    BR,

    IF,
    
    END,

    CALL,
    RETURN
};

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

union InstrArgs {
	// For numeric instructions
	Value value;	

	// For `loop`s and `block`s
	BlockInfo block;

	struct {
		BlockInfo if_block_info;
		std::optional<BlockInfo> else_block_info;
		size_t end_instr; // index into the end instruction of the if block
	} if_;

	// for br, call
	size_t index;
};

struct Instruction {
    InstrKind kind;

    std::optional<InstrArgs> args;

    Instruction(InstrKind k):
        kind(k), args(std::nullopt) {}

    Instruction(InstrKind k, InstrArgs a):
        kind(k), args(a) {}

    std::string to_string();
};

#endif
