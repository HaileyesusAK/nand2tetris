#include <bitset>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "instruction.hpp"
#include "symbol_table.hpp"

AInstruction::AInstruction(const std::string& _symbol) : symbol(_symbol) {}

MachineCode AInstruction::decode(const SymbolTable<uint16_t>& symbolTable) {
    if(std::all_of(symbol.begin(), symbol.end(), ::isdigit))
        return static_cast<uint16_t>(std::stoul(symbol));
    else
        return symbolTable.get(symbol);
}


CInstruction::CInstruction(const std::string& s) : inst(s) {}

MachineCode CInstruction::decode(const SymbolTable<uint16_t>& symbolTable) {
    std::string dst("null"), jmp("null"), comp;
    auto j = inst.find(';');
    if(j != std::string::npos)
        jmp = inst.substr(j+1);

    auto i = inst.find('=');
    if(i != std::string::npos) {
        comp = inst.substr(i+1, j-i-1);
        dst = inst.substr(0, i);
    }
    else
        comp = inst.substr(0, j);

    uint16_t prefix = 7 << 13;
    try {
        auto dstCode = dstMap.at(dst) << 3;
        auto compCode = compMap.at(comp) << 6;
        auto jmpCode = jmpMap.at(jmp);
        return static_cast<uint16_t>(prefix | compCode | dstCode | jmpCode);
    }
    catch(std::out_of_range& e) {
        throw std::domain_error("Invalid instruction: " + inst);
    }
}

std::shared_ptr<Instruction> InstructionFactory::create(const std::string& s) {
    if(s.front() == '@')
        return std::make_shared<AInstruction>(s.substr(1));
    else
        return std::make_shared<CInstruction>(s);
}
