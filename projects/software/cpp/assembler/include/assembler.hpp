#ifndef __ASSEMBLER_HPP__
#define __ASSEMBLER_HPP__

#include <bitset>
#include <fstream>
#include <string>
#include <vector>
#include "symbol_table.hpp"

using InstCodeMap = std::unordered_map<std::string, uint16_t>;

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

const uint8_t WSIZE = 16;

class Assembler {
	private:
		SymbolTable<uint16_t> symTable; 
		std::vector<AsmInstruction> asmInstructions;
		static const uint16_t BASE_ADDR = 16;

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
		Assembler(std::ifstream& inFile);
		static std::string compact(const std::string&);
		static AsmInstruction extractInst(const std::string&);
		static uint16_t generate(const std::string& c_inst);
		std::vector<std::bitset<WSIZE>> generate();
};

#endif
