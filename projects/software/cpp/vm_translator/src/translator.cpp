#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include "generator.hpp"
#include "translator.hpp"

VmTranslator::VmTranslator(const std::string& inFileName) {
    this->inFile = std::ifstream(inFileName);    
    this->generator["add"] = std::unique_ptr<Generator>(new AddGenerator());
    this->generator["and"] = std::unique_ptr<Generator>(new AndGenerator());
    this->generator["eq"] = std::unique_ptr<Generator>(new EqGenerator());
    this->generator["gt"] = std::unique_ptr<Generator>(new GtGenerator());
    this->generator["lt"] = std::unique_ptr<Generator>(new LtGenerator());
    this->generator["neg"] = std::unique_ptr<Generator>(new NegGenerator());
    this->generator["not"] = std::unique_ptr<Generator>(new NotGenerator());
    this->generator["or"] = std::unique_ptr<Generator>(new OrGenerator());
    this->generator["pop"] = std::unique_ptr<Generator>(new StackPushGenerator());
    this->generator["push"] = std::unique_ptr<Generator>(new StackPushGenerator());
    this->generator["sub"] = std::unique_ptr<Generator>(new SubGenerator());
}
