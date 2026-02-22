#ifndef INSTRUCTION_HPP 
#define INSTRUCTION_HPP

#include <optional>
#include <value.hpp>

enum class InstrKind {
    I32_CONST,
    I32_ADD,
    I32_SUB,
    I32_MUL,

    BLOCK,
    LOOP,
    BR,

    IF,
    
    END
};

typedef struct {
	size_t block_start; // index into the start of the block (i.e, the instruction which creates the block and NOT the first instruction of the block)
	size_t block_end; // index into the `end` instruction of the block 

}BlockInfo;

union InstrArgs {
	Value value;	

	// For `loop`s, `block`s, `if`s, 'else's
	BlockInfo block_kind;

	struct {
		BlockInfo if_block_info;
		std::optional<BlockInfo> else_block_info;
	} if_;
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
