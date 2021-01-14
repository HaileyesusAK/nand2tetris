#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

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

class VmTranslator {
    private:
        CommandMap commandMap;
        GeneratorMap generator;
    public:
        VmTranslator();
        AsmInst translate(const std::string& fileName);
        AsmInst translate(const std::vector<std::string>& parameters);
};

#endif
