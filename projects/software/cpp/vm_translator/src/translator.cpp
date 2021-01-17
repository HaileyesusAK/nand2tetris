#include <filesystem>
#include <fstream>
#include <cerrno>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "generator.hpp"
#include "translator.hpp"

namespace fs = std::filesystem;

std::vector<std::string> split(const std::string& s) {
    std::vector<std::string> words;
    std::string word;
    std::stringstream ss(s);
    while(getline(ss, word, ' '))
        if(word.length() > 0)
            words.push_back(word);
    return words;
}

VmTranslator::VmTranslator(): commandMap {{"push", Command::PUSH}, {"pop", Command::POP},
                                          {"add", Command::ADD}, {"sub", Command::SUB},
                                          {"lt", Command::LT}, {"gt", Command::GT}, {"eq", Command::EQ},
                                          {"neg", Command::NEG}, {"not", Command::NOT},
                                          {"call", Command::CALL}, {"function", Command::FUNCTION},
                                          {"return", Command::RET}, {"label", Command::LABEL},
                                          {"goto", Command::GOTO}, {"or", Command::OR},
                                          {"and", Command::AND}, {"if-goto", Command::IF_GOTO}},

                               generator {{"add", std::make_shared<AddGenerator>()},
                                          {"sub", std::make_shared<SubGenerator>()},
                                          {"and", std::make_shared<AndGenerator>()},
                                          {"eq", std::make_shared<EqGenerator>()},
                                          {"gt", std::make_shared<GtGenerator>()},
                                          {"lt", std::make_shared<LtGenerator>()},
                                          {"neg", std::make_shared<NegGenerator>()},
                                          {"not", std::make_shared<NotGenerator>()},
                                          {"or", std::make_shared<OrGenerator>()},
                                          {"pop", std::make_shared<StackPopGenerator>()},
                                          {"push", std::make_shared<StackPushGenerator>()},
                                          {"if-goto", std::make_shared<IfGotoGenerator>()},
                                          {"goto", std::make_shared<GotoGenerator>()},
                                          {"call", std::make_shared<CallGenerator>()},
                                          {"function", std::make_shared<FunctionGenerator>()},
                                          {"return", std::make_shared<ReturnGenerator>()},
                                          {"label", std::make_shared<LabelGenerator>()}}{}

AsmInst VmTranslator::translate(const std::vector<std::string>& parameters) {
    AsmInst insts;
    auto cmd = parameters.at(0);
    auto code_generator = generator.at(cmd);

    switch(commandMap.at(cmd)) {
        case Command::FUNCTION:
        {
            funcName = parameters.at(1);
            auto nLocals = static_cast<uint16_t>(std::stoul(parameters.at(2)));
            insts = code_generator->generate(funcName, nLocals);
        }
        break;

        case Command::CALL:
        {
            auto nArgs = static_cast<uint16_t>(std::stoul(parameters.at(2)));
            insts = code_generator->generate(parameters.at(1), nArgs);
        }
        break;

        case Command::LABEL:
        case Command::GOTO:
        case Command::IF_GOTO:
            insts = code_generator->generate(funcName, parameters.at(1));
        break;

        case Command::PUSH:
        case Command::POP:
        {
            auto idx = static_cast<uint16_t>(std::stoul(parameters.at(2)));
            insts = code_generator->generate(parameters.at(1), idx);
        }
        break;

        case Command::RET:
            insts = code_generator->generate();
        break;

        default:
            insts = code_generator->generate();
        break;
    }

    return insts;
}

AsmInst VmTranslator::translate_file(const fs::path& filePath) {
    std::ifstream inFile(filePath);
    if(!inFile.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + filePath.string());

    AsmInst instructions;
    std::string line, s;
    std::vector<std::string> parameters;
    StackGenerator::setStaticFileName(filePath.filename());

    while(std::getline(inFile, s)) {
        std::string line(s.substr(0, s.find_first_of('/')));
        line = line.substr(0, s.find_first_of('\r'));
        parameters = split(line);
        if(parameters.empty())
            continue;

        instructions.push_back("//" + line);
        auto insts = translate(parameters);
        std::copy(insts.begin(), insts.end(), std::back_inserter(instructions));
    }

    return instructions;
}

void VmTranslator::saveAsm(const AsmInst& insts, fs::path path) {
    if(fs::is_directory(path))
        path = path / path.filename();

    path.replace_extension(".asm");
    std::ofstream file(path.string());

    for(const auto& inst: insts) {
        if(inst.front() != '/' && inst.front() != '(')
            file << "\t" << inst << std::endl;
        else
            file << inst << std::endl;
    }
}

void VmTranslator::translate(const fs::path& path) {
    std::vector<fs::path> paths;
    AsmInst insts;
    Generator generator;

    if(fs::is_directory(path)) {
        auto dir_iter = fs::directory_iterator(path);
        auto dir_end = fs::directory_iterator();
        
        auto vm_filter = [](const fs::path& path) {
            return !fs::is_directory(path) && path.extension() == ".vm";
        };
        std::copy_if(dir_iter, dir_end, back_inserter(paths), vm_filter);

        // Add bootstrap code
        for(auto& inst: generator.generateBootstrap())
            insts.push_back(inst);
        
    }
    else {
        paths.push_back(path);
    }
    
    for(auto& path: paths) {
        for(auto& inst: translate_file(path)) {
            insts.push_back(inst);
        }
    }

    // Add closing code
    for(auto& inst: generator.generateClose())
        insts.push_back(inst);

    saveAsm(insts, path);
}
