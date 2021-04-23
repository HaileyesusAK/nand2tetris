#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "assembler.hpp"


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

    auto dstCode = Assembler::dstMap.at(dst);
    auto compCode = Assembler::compMap.at(comp);
    auto &jmpCode = Assembler::jmpMap.at(jmp);
    return (7<<13) | (compCode<<6) | (dstCode<<3) | jmpCode;
}

Assembler::Assembler(std::ifstream& inFile) {
    AsmInstruction inst;
    uint16_t pc = 0;
    std::string line;

    symTable.set("SP", 0);
    symTable.set("LCL", 1);
    symTable.set("ARG", 2);
    symTable.set("THIS", 3);
    symTable.set("THAT", 4);
    symTable.set("SCREEN", 16384);
    symTable.set("KEYBOARD", 24576);
    for(int i = 0; i < 16; ++i)
        symTable.set(std::string("R") + std::to_string(i), i);

    while(std::getline(inFile, line)) {
        inst = extractInst(line);
        switch(inst.type) {
            case AsmInstType::Label:
                symTable.set(inst.str, pc);
            break;

            case AsmInstType::AInst:
                if(isNumber(inst.str)) { //Treat the number as a symbol
                    auto addr = static_cast<uint16_t>(std::stoul(inst.str));
                    symTable.set(inst.str, addr);
                }
                asmInstructions.push_back(inst);
                pc++;
            break;

            case AsmInstType::CInst:
                asmInstructions.push_back(inst);
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

    for(const auto& inst: asmInstructions) {
        if(inst.type == AsmInstType::AInst) {
            if(!symTable.has(inst.str))
            {
                symTable.set(inst.str, variableAddr);
                variableAddr++;
            }
            machineCode = symTable.get(inst.str);
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

            auto dstCode = Assembler::dstMap.at(dst);
            auto compCode = Assembler::compMap.at(comp);
            auto jmpCode = Assembler::jmpMap.at(jmp);
            machineCode = (7<<13) | (compCode<<6) | (dstCode<<3) | jmpCode;
        }
        machineInstructions.push_back(std::bitset<16>(machineCode));
    }

    return machineInstructions;
}
