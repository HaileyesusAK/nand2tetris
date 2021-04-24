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
    symTable.get("radius");

    auto inst = InstructionFactory::create("@radius", 0, symTable);
    MachineCode machineCode;
    inst->decode(machineCode);

    //@radius encodes into 0000000000010000 which is 16
    CHECK_EQUAL(16, machineCode.to_ulong());
}

TEST(Instruction, Label_Instruction)
{
    SymbolTable<uint16_t> symTable;

    auto inst = InstructionFactory::create("(END_LOOP)", 8080, symTable);
    MachineCode machineCode(255);
    inst->decode(machineCode);

    // The label must be treated as a symbol
    CHECK_EQUAL(true, symTable.has("END_LOOP"));

    // A label instruction is not encoded; hence, the expected maching code remains unchanged.
    CHECK_EQUAL(255, machineCode.to_ulong());

    // the symbol END_LOOP must be assigned the program counter as its address
    CHECK_EQUAL(8080, symTable.get("END_LOOP"));
}

TEST(Instruction, C_Instruction)
{
    SymbolTable<uint16_t> symTable;

    // must be encoded as 111 0 001101 000 000
    auto inst = InstructionFactory::create("!D", 0, symTable);
    MachineCode code;
    inst->decode(code);
    CHECK_EQUAL(MachineCode("1110001101000000").to_ulong(), code.to_ulong());

    // must be encoded as 111 1 010101 000 101
    inst = InstructionFactory::create("D|M;JNE", 0, symTable);
    inst->decode(code);
    CHECK_EQUAL(MachineCode("1111010101000101").to_ulong(), code.to_ulong());

    // must be encoded as 111 0 110111 101 000
    inst = InstructionFactory::create("AM=A+1", 0, symTable);
    inst->decode(code);
    CHECK_EQUAL(MachineCode("1110110111101000").to_ulong(), code.to_ulong());

    // must be encoded as 111 1 000000 111 010
    inst = InstructionFactory::create("AMD=D&M;JEQ", 0, symTable);
    inst->decode(code);
    CHECK_EQUAL(MachineCode("1111000000111010").to_ulong(), code.to_ulong());
}
