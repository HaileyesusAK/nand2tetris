#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <bitset>

#include "assembler.hpp"

using CodeMap = std::unordered_map<std::string, uint16_t>;

static const CodeMap dstMap {
        {"null", 0}, {"M", 1}, {"D", 2}, {"MD", 3},
        {"A", 4}, {"AM", 5}, {"AD", 6}, {"AMD", 7}
};

static const CodeMap jmpMap {
        {"null", 0}, {"JGT", 1}, {"JEQ", 2}, {"JGE", 3},
        {"JLT", 4}, {"JNE", 5}, {"JLE", 6}, {"JMP", 7}
};

static const CodeMap compMap {
        {"0", 42}, {"1", 63}, {"-1", 58}, {"D", 12}, {"A", 48}, {"!D", 13},
        {"!A", 49}, {"-D", 15}, {"-A", 51}, {"D+1", 31}, {"A+1", 55},
        {"D-1", 14}, {"A-1", 50}, {"D+A", 2}, {"D-A", 19}, {"A-D", 7},
        {"D&A", 0}, {"D|A", 21}, {"M", 112}, {"!M", 113}, {"-M", 115},
        {"M+1", 119}, {"M-1", 114}, {"D+M", 66}, {"D-M", 83}, {"M-D", 71},
        {"D&M", 64}, {"D|M", 85}
};

std::string Assembler::compact(const std::string& s) {
    std::string line(s.substr(0, s.find_first_of('/')));
    auto end_pos = std::remove_if(line.begin(), line.end(), ::isspace);
    line.erase(end_pos, line.end());
    return line;
}

static bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

AsmInstruction Assembler::extractInst(const std::string& line) {
    AsmInstruction inst;

    inst.str = Assembler::compact(line);
    if(inst.str.empty() || inst.str.front() == '/')
        inst.type = AsmInstType::Blank;
    else if(inst.str.front() == '(' && inst.str.back() == ')') {
        inst.type = AsmInstType::Label;
        inst.str.erase(0, 1);
        inst.str.pop_back();
    }
    else if(*inst.str.begin() == '@') {
        inst.type = AsmInstType::AInst;
        inst.str.erase(0, 1);
    }
    else
        inst.type = AsmInstType::CInst;

    return inst;
}

uint16_t Assembler::generate(const std::string& c_inst) {
    std::string str = Assembler::compact(c_inst);
    std::string dst("null"), jmp("null"), comp;
    auto semi_pos = str.find(';');
    if(semi_pos != std::string::npos)
        jmp = str.substr(semi_pos + 1, str.length() - semi_pos - 1);

    auto eq_pos = str.find('=');
    if(eq_pos != std::string::npos) {
        dst = str.substr(0, eq_pos);
        comp = str.substr(eq_pos + 1, semi_pos - eq_pos - 1);
    }
    else
        comp = str.substr(0, semi_pos);

    auto &dstCode = dstMap.at(dst);
    auto &compCode = compMap.at(comp);
    auto &jmpCode = jmpMap.at(jmp);
    return (7<<13) | (compCode<<6) | (dstCode<<3) | jmpCode;
}

Assembler::Assembler(std::ifstream& inFile) {
    AsmInstruction inst;
    uint16_t pc = 0;
    std::string line;

    this->symTable.set("SP", 0);
    this->symTable.set("LCL", 1);
    this->symTable.set("ARG", 2);
    this->symTable.set("THIS", 3);
    this->symTable.set("THAT", 4);
    this->symTable.set("SCREEN", 16384);
    this->symTable.set("SCREEN", 24576);
    for(uint16_t i = 0; i < 16; ++i)
        this->symTable.set(std::string("R") + std::to_string(i), i);

    while(std::getline(inFile, line)) {
        inst = extractInst(line);
        switch(inst.type) {
            case AsmInstType::Label:
                this->symTable.set(inst.str, pc);
            break;

            case AsmInstType::AInst:
                if(isNumber(inst.str)) { //Treat the number as a symbol
                    auto addr = static_cast<uint16_t>(std::stoul(inst.str));
                    this->symTable.set(inst.str, addr);
                }
                this->asmInstructions.push_back(inst);
                pc++;
            break;

            case AsmInstType::CInst:
                this->asmInstructions.push_back(inst);
                pc++;
            break;

            default:
                break;
        }
    }
}

std::vector<std::bitset<16>> Assembler::generate() {
    std::vector<std::bitset<16>> machineInstructions;
    uint16_t machineCode;
    uint16_t variableAddr = Assembler::BASE_ADDR;

    for(const auto& inst: this->asmInstructions) {
        if(inst.type == AsmInstType::AInst) {
            if(!this->symTable.has(inst.str))
            {
                this->symTable.set(inst.str, variableAddr);
                variableAddr++;
            }
            machineCode = this->symTable.get(inst.str);
        }
        else {
            std::string dst("null"), jmp("null"), comp;
            auto semi_pos = inst.str.find(';');
            if(semi_pos != std::string::npos)
                jmp = inst.str.substr(semi_pos + 1, inst.str.length() - semi_pos - 1);

            auto eq_pos = inst.str.find('=');
            if(eq_pos != std::string::npos) {
                dst = inst.str.substr(0, eq_pos);
                comp = inst.str.substr(eq_pos + 1, semi_pos - eq_pos - 1);
            }
            else
                comp = inst.str.substr(0, semi_pos);

            auto &dstCode = dstMap.at(dst);
            auto &compCode = compMap.at(comp);
            auto &jmpCode = jmpMap.at(jmp);
            machineCode = (7<<13) | (compCode<<6) | (dstCode<<3) | jmpCode;
        }
        machineInstructions.push_back(std::bitset<16>(machineCode));
    }

    return machineInstructions;
}
