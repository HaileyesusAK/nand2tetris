#include <algorithm>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "assembler.hpp"
#include "instruction.hpp"

namespace fs = std::filesystem;

std::string Assembler::compact(const std::string& s) {
    std::string line(s.substr(0, s.find_first_of('/')));
    auto end_pos = std::remove_if(line.begin(), line.end(), ::isspace);
    line.erase(end_pos, line.end());
    return line;
}

Assembler::Assembler(const fs::path& path) : inputPath(path) {}

SymbolTable<uint16_t> Assembler::generateSymbolTable(const fs::path& path) {
    SymbolTable<uint16_t> symbolTable;

    std::ifstream inFile(path);
    if(!inFile.is_open())
       throw std::runtime_error(std::strerror(errno) + std::string(": ") + inputPath.string());

    symbolTable.set("SP", 0);
    symbolTable.set("LCL", 1);
    symbolTable.set("ARG", 2);
    symbolTable.set("THIS", 3);
    symbolTable.set("THAT", 4);
    symbolTable.set("SCREEN", 16384);
    symbolTable.set("KBD", 24576);

    // Set the 16 predefined symbols: R0 - R15
    for(int i = 0; i < 16; ++i)
        symbolTable.set("R" + std::to_string(i), static_cast<uint16_t>(i));

    // Extract symbols from the source file and extend the symbol table
    uint16_t pc = 0;
    std::string line, s;
    std::vector<std::string> unresolvedSymbols;
    while(std::getline(inFile, line)) {
        s = compact(line);
        if(s.empty())
            continue;

        if(s.front() == '(') {
            symbolTable.set(s.substr(1, s.size() - 2), pc);
        }
        else {
            pc++;
            if(s.front() == '@' && !std::all_of(s.begin() + 1, s.end(), ::isdigit))
                unresolvedSymbols.push_back(s.substr(1));
        }
    }

    uint16_t varAddr = 16;  //Variables are allocated from Memory address 16 onwards
    for(const auto& symbol : unresolvedSymbols) {
        if(symbolTable.set(symbol, varAddr))
            varAddr++;
    }

    return symbolTable;
}

void Assembler::generate(const fs::path& asmPath) {
    std::ifstream inFile(asmPath);
    if(!inFile.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + asmPath.string());

    auto hackPath = asmPath;
    std::ofstream outFile(hackPath.replace_extension(".hack"));
    if(!outFile.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + hackPath.string());

    SymbolTable symbolTable = generateSymbolTable(asmPath);

    std::string line, s;
    while(std::getline(inFile, line)) {
        s = compact(line);
        if(s.empty() || s.front() == '(')
            continue;

        auto instPtr = InstructionFactory::create(s);
        outFile << instPtr->decode(symbolTable) << std::endl;
    }
}

void Assembler::generate() {
    if(fs::is_directory(inputPath)) {
        std::for_each(fs::directory_iterator(inputPath),
                      fs::directory_iterator(),
                      [this](const fs::path& p) {
                            if(!fs::is_directory(p) && p.extension() == ".asm") {
                                this->generate(p);
                            }
                      });
    }
    else {
        auto assembler = Assembler(inputPath);
        assembler.generate(inputPath);
    }
}
