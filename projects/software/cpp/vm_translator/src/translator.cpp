#include <fstream>
#include <cerrno>
#include <memory>
#include <regex>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "generator.hpp"
#include "translator.hpp"

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
                                          {"ret", Command::RET}, {"label", Command::LABEL},
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
                                          {"label", std::make_shared<LabelGenerator>()}}{}

AsmInst VmTranslator::translate(const std::vector<std::string>& parameters) {
    AsmInst insts;
    std::string funcName;
    std::stack<std::string> callStack;
    auto cmd = parameters.at(0);
    auto code_generator = generator.at(cmd);

    switch(commandMap.at(cmd)) {
        case Command::FUNCTION:
        {
            callStack.push(parameters.at(1));
            auto funcName = callStack.top();
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
        {
            auto funcName = callStack.top();
            insts = code_generator->generate(funcName, parameters.at(1));
        }
        break;

        case Command::PUSH:
        case Command::POP:
        {
            auto idx = static_cast<uint16_t>(std::stoul(parameters.at(2)));
            insts = code_generator->generate(parameters.at(1), idx);
        }
        break;

        case Command::RET:
            callStack.pop();
            insts = code_generator->generate();
        break;

        default:
            insts = code_generator->generate();
        break;
    }

    return insts;
}

AsmInst VmTranslator::translate(const std::string& fileName) {
    std::ifstream inFile(fileName);
    if(!inFile.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + fileName);

    AsmInst instructions;
    std::string line, s;
    std::vector<std::string> parameters;
    StackGenerator::setStaticFileName(fileName);

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
