#include <bitset>
#include <iostream>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "instruction.hpp"
#include "symbol_table.hpp"

/**************************** AInstruction ******************************/
AInstruction::AInstruction(const std::string& _symbol, SymbolTable<uint16_t>& _symbolTable) :
    symbol(_symbol),
    symbolTable(_symbolTable) {}

void AInstruction::decode(MachineCode& machineCode) {
    machineCode = MachineCode(symbolTable.get(symbol));
}

InstructionType AInstruction::type() {return InstructionType::A_INST; }
/************************************************************************/


/**************************** CInstruction ******************************/
CInstruction::CInstruction(const std::string& s) : inst(s) {}

void CInstruction::decode(MachineCode &code) {
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
        code = static_cast<uint16_t>(prefix | compCode | dstCode | jmpCode);
    }
    catch(std::out_of_range& e) {
        throw std::domain_error("Invalid instruction: " + inst);
    }
}

InstructionType CInstruction::type() {return InstructionType::C_INST; }
/**********************************************************************/


/************************* LabelInstruction ***************************/
LabelInstruction::LabelInstruction(const std::string& _label, uint16_t _pc, SymbolTable<uint16_t>& _symbolTable) :
    label(_label),
    pc(_pc),
    symbolTable(_symbolTable) {}

void LabelInstruction::decode(MachineCode& machineCode) {
    symbolTable.set(label, pc);
}

InstructionType LabelInstruction::type() {return InstructionType::LABEL_INST; }
/**********************************************************************/


/********************** InstructionFactory ***************************/
std::shared_ptr<Instruction> InstructionFactory::create(const std::string& s,
                                                        uint16_t pc,
                                                        SymbolTable<uint16_t>& symbolTable)
{
    if(s.front() == '(' && s.back() == ')')
        return std::make_shared<LabelInstruction>(s.substr(1, s.size() - 2), pc, symbolTable);
    else if(s.front() == '@')
        return std::make_shared<AInstruction>(s.substr(1), symbolTable);
    else
        return std::make_shared<CInstruction>(s);
}
/**********************************************************************/
