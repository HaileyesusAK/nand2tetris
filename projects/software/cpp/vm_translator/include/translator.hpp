#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include "generator.hpp"

using GeneratorMap = std::unordered_map<std::string, std::unique_ptr<Generator>>;

class VmTranslator {
    private:
        GeneratorMap generator;
        std::ifstream inFile; 

    public:
        VmTranslator(const std::string& inFileName);
        AsmInst translate();
};

#endif
