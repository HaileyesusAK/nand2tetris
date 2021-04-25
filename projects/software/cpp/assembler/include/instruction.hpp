#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <bitset>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "symbol_table.hpp"

const uint8_t WSIZE = 16;
using InstCodeMap = std::unordered_map<std::string, uint16_t>;
using MachineCode = std::bitset<WSIZE>;

enum struct InstructionType {
    A_INST,
    C_INST,
    LABEL_INST
};

class Instruction {
    public:
        virtual MachineCode decode(const SymbolTable<uint16_t>& symbolTable) = 0;
};

class AInstruction : public Instruction {
    private:
        std::string symbol;

    public:
        AInstruction(const std::string& symbol);
        MachineCode decode(const SymbolTable<uint16_t>& symbolTable);
};

class CInstruction : public Instruction {
    private:
        std::string inst;

		/* mappings between an instruction part with its binary representation. The binary
		   representation of the decimal values correspond to a valid JACK instruction part
		*/
		static inline const InstCodeMap compMap {
			{"0", 42}, {"1", 63}, {"-1", 58}, {"D", 12}, {"A", 48}, {"!D", 13},
			{"!A", 49}, {"-D", 15}, {"-A", 51}, {"D+1", 31}, {"A+1", 55},
			{"D-1", 14}, {"A-1", 50}, {"D+A", 2}, {"D-A", 19}, {"A-D", 7},
			{"D&A", 0}, {"D|A", 21}, {"M", 112}, {"!M", 113}, {"-M", 115},
			{"M+1", 119}, {"M-1", 114}, {"D+M", 66}, {"D-M", 83}, {"M-D", 71},
			{"D&M", 64}, {"D|M", 85}
		};

		static inline const InstCodeMap dstMap {
			{"null", 0}, {"M", 1}, {"D", 2}, {"MD", 3},
			{"A", 4}, {"AM", 5}, {"AD", 6}, {"AMD", 7}
		};

		static inline const InstCodeMap jmpMap {
			{"null", 0}, {"JGT", 1}, {"JEQ", 2}, {"JGE", 3},
			{"JLT", 4}, {"JNE", 5}, {"JLE", 6}, {"JMP", 7}
		};

    public:
        CInstruction(const std::string& inst);
        MachineCode decode(const SymbolTable<uint16_t>& symbolTable);
};

class InstructionFactory {
    public:
       static std::shared_ptr<Instruction> create(const std::string& instruction);
};

#endif
