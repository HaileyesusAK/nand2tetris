#include <iostream>
#include <stdexcept>

#include "instruction.hpp"
#include "symbol_table.hpp"
#include "utils.hpp"

#include <CppUTest/TestHarness.h>

using namespace std;

TEST_GROUP(Instruction)
{

};

TEST(Instruction, A_Instruction)
{
    SymbolTable<uint16_t> symTable;
    uint16_t addr = 16;
    symTable.set("radius", addr);

    auto inst = InstructionFactory::create("@radius");

    //@radius encodes into 0000000000010000 which is 16
    CHECK_EQUAL(addr, inst->decode(symTable).to_ulong());
}

TEST(Instruction, C_Instruction)
{
    SymbolTable<uint16_t> symTable;

    // must be encoded as 111 0 001101 000 000
    auto inst = InstructionFactory::create("!D");
    CHECK_EQUAL(MachineCode("1110001101000000").to_ulong(), inst->decode(symTable).to_ulong());

    // must be encoded as 111 1 010101 000 101
    inst = InstructionFactory::create("D|M;JNE");
    CHECK_EQUAL(MachineCode("1111010101000101").to_ulong(), inst->decode(symTable).to_ulong());

    // must be encoded as 111 0 110111 101 000
    inst = InstructionFactory::create("AM=A+1");
    CHECK_EQUAL(MachineCode("1110110111101000").to_ulong(), inst->decode(symTable).to_ulong());

    // must be encoded as 111 1 000000 111 010
    inst = InstructionFactory::create("AMD=D&M;JEQ");
    CHECK_EQUAL(MachineCode("1111000000111010").to_ulong(), inst->decode(symTable).to_ulong());
}

TEST(Instruction, Invalid_Instruction)
{
    SymbolTable<uint16_t> symTable;

    // must throw an exception due to unknown Comp part
    auto inst = InstructionFactory::create("~D");
    CHECK_THROWS(std::domain_error, inst->decode(symTable));

    // must throw an exception due to unknown dst part
    inst = InstructionFactory::create("MDA=!D");
    CHECK_THROWS(std::domain_error, inst->decode(symTable));

    // must throw an exception due to unknown jmp part
    inst = InstructionFactory::create("AMD=!D;JQE");
    CHECK_THROWS(std::domain_error, inst->decode(symTable));
}
