#pragma once
#include<cstdint>

enum class MemFlag : uint8_t
{
    Address32_NoMax             = 0x00,
    Address32_MaxExists         = 0x01,
    Address64_NoMax             = 0x04,
    Address64_MaxExists         = 0x05,
};

enum class Flag : uint8_t
{
    ActiveImplicit          = 0x00,  // active, table 0, func indices
    Passive                 = 0x01,  // passive, func indices
    ActiveExplicit          = 0x02,  // active, explicit table, func indices
    Declarative             = 0x03,  // declarative, func indices
    ActiveImplicitExpr      = 0x04,  // active, table 0, expressions
    PassiveExpr             = 0x05,  // passive, expressions
    ActiveExplicitExpr      = 0x06,  // active, explicit table, expressions
    DeclarativeExpr         = 0x07,  // declarative, expressions
};