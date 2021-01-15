#include <array>
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>
#include "utils.hpp"

std::pair<std::string, int> execute(const std::string& c) {
    std::array<char, 128> buffer;
    std::string result;
    std::string cmd(c + " 2>&1");
    auto pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }
    
    return std::make_pair(result, pclose(pipe));
}

void saveAsm(const AsmInst& insts, const std::string& filename) {
    std::ofstream outFile(std::string("../data/") + filename);

    for(auto& inst: insts) {
        if(inst.front() == '(')
            outFile << inst << std::endl;
        else
            outFile << "\t" << inst << std::endl;
    }
}

std::pair<std::string, int> test(const std::string& filename, const AsmInst& insts) {
    std::string asmFile = filename + ".asm";
    saveAsm(insts, asmFile);
    std::string tstFile = filename + ".tst";
    return execute(std::string("CPUEmulator ") + tstFile);
}
