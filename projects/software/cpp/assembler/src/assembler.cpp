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

Assembler::Assembler(const fs::path& path) : inputPath(path) {
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

    for(int i = 0; i < 16; ++i)
        symbolTable.set(std::string("R") + std::to_string(i), i);

    //Build symbol table
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
}

void Assembler::generate() {
    std::ifstream inFile(inputPath);
    if(!inFile.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + inputPath.string());

    auto outputPath = inputPath;
    std::ofstream outFile(outputPath.replace_extension(".hack"));
    if(!outFile.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + outputPath.string());

    //Encode each assembly instruction and write to the output file
    std::string line, s;
    while(std::getline(inFile, line)) {
        s = compact(line);
        if(s.empty() || s.front() == '(')
            continue;

        auto instPtr = InstructionFactory::create(s);
        outFile << instPtr->decode(symbolTable) << std::endl;
    }
}
