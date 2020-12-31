#ifndef __ASSEMBLER_HPP__
#define __ASSEMBLER_HPP__

#include <bitset>
#include <fstream>
#include <string>
#include <vector>
#include "symboltable.hpp"

enum struct AsmInstType {
    AInst,
    Blank,
    CInst,
    Label
};

struct AsmInstruction{
    std::string str;
    AsmInstType type;
};

const uint8_t WSIZE = sizeof(uint16_t) * 8;

class Assembler {
    private:
       SymbolTable<uint16_t> symTable; 
       std::vector<AsmInstruction> asmInstructions;
       static const uint16_t BASE_ADDR = 16;
       static const uint8_t WSIZE = sizeof(uint16_t) * 8;

    public:
        Assembler(std::ifstream& inFile);

        static std::string compact(const std::string&);
        static AsmInstruction extractInst(const std::string&);
        static uint16_t generate(const std::string& c_inst);
        std::vector<std::bitset<WSIZE>> generate();
};

#endif
