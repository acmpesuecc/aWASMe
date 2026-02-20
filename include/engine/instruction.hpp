#ifndef INSTRUCTION_HPP 
#define INSTRUCTION_HPP

#include <optional>
#include <value.hpp>

enum class InstrKind {
    I32_CONST,
    I32_ADD,
    I32_SUB,
    I32_MUL,
    END
};

struct Instruction {
    InstrKind kind;
    std::optional<Value> operand;

    Instruction(InstrKind k):
        kind(k), operand(std::nullopt) {}

    Instruction(InstrKind k, Value v):
        kind(k), operand(v) {}
};

#endif