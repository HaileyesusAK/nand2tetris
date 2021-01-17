#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "generator.hpp"

enum class Command {
    PUSH, POP, ADD, SUB, AND, OR, LT, GT, EQ, NEG, NOT,
    FUNCTION, CALL, RET, LABEL, GOTO, IF_GOTO
};

using CommandMap = std::unordered_map<std::string, Command>;
using GeneratorMap = std::unordered_map<std::string, std::shared_ptr<Generator>>;
namespace fs = std::filesystem;

class VmTranslator {
    private:
        CommandMap commandMap;
        GeneratorMap generator;
        std::string funcName;
        void saveAsm(const AsmInst& insts, fs::path path);
        AsmInst translate_file(const fs::path& path);
        AsmInst translate(const std::vector<std::string>& parameters);

    public:
        VmTranslator();
        void translate(const fs::path& path);
};

#endif
