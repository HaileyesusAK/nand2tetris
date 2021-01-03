#include <fstream>
#include <memory>
#include <string>
#include <unordered_map>
#include "generator.hpp"
#include "translator.hpp"

VmTranslator::VmTranslator(const std::string& inFileName) {
    this->inFile = std::ifstream(inFileName);    

    StackCodeMap stackMap;
    stackMap["argument"] = std::unique_ptr<Segment>(new ArgumentSegment());
    stackMap["constant"] = std::unique_ptr<Segment>(new ConstantSegment());
    stackMap["local"] = std::unique_ptr<Segment>(new LocalSegment());
    stackMap["pointer"] = std::unique_ptr<Segment>(new PointerSegment());
    stackMap["static"] = std::unique_ptr<Segment>(new StaticSegment(inFileName));
    stackMap["temp"] = std::unique_ptr<Segment>(new TempSegment());
    stackMap["that"] = std::unique_ptr<Segment>(new ThatSegment());
    stackMap["this"] = std::unique_ptr<Segment>(new ThisSegment());

    this->generator["add"] = std::unique_ptr<Generator>(new AddGenerator());
    this->generator["and"] = std::unique_ptr<Generator>(new AndGenerator());
    this->generator["eq"] = std::unique_ptr<Generator>(new EqGenerator());
    this->generator["gt"] = std::unique_ptr<Generator>(new GtGenerator());
    this->generator["lt"] = std::unique_ptr<Generator>(new LtGenerator());
    this->generator["neg"] = std::unique_ptr<Generator>(new NegGenerator());
    this->generator["not"] = std::unique_ptr<Generator>(new NotGenerator());
    this->generator["or"] = std::unique_ptr<Generator>(new OrGenerator());
    this->generator["pop"] = std::unique_ptr<Generator>(new StackPushGenerator(stackMap));
    this->generator["push"] = std::unique_ptr<Generator>(new StackPushGenerator(stackMap));
    this->generator["sub"] = std::unique_ptr<Generator>(new SubGenerator());
}
