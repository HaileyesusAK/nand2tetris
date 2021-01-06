#define CATCH_CONFIG_MAIN
#include <fstream>
#include <iostream>
#include <string>
#include "generator.hpp"
#include "utils.hpp"
#include "catch.hpp"

using namespace std;

static void saveAsm(const AsmInst& insts, const string& filename) {
    ofstream outFile(string("../data/") + filename);

    for(auto& inst: insts) {
        if(inst.front() == '(')
            outFile << inst << endl;
        else
            outFile << "\t" << inst << endl;
    }
}

static pair<string, int> test(const string& filename, const AsmInst& insts) {
    string asmFile = filename + ".asm";
    saveAsm(insts, asmFile);
    string tstFile = filename + ".tst";
    return execute(string("CPUEmulator ") + tstFile);
}

static pair<string, int> test_push(const StackCodeMap& stackMap, const string& segment, uint16_t idx) {
    auto insts = stackMap.at(segment).get()->push(idx);
    string filename(string("../data/push") + segment);
    return test(filename, insts);
}

static pair<string, int> test_pop(const StackCodeMap& stackMap, const string& segment, uint16_t idx) {
    auto insts = stackMap.at(segment).get()->pop(idx);
    string filename(string("../data/pop") + segment);
    return test(filename, insts);
}

TEST_CASE("Test stack operations", "[stack]") {

    StackCodeMap stackMap;
    stackMap["argument"] = std::unique_ptr<Segment>(new ArgumentSegment());
    stackMap["constant"] = std::unique_ptr<Segment>(new ConstantSegment());
    stackMap["local"] = std::unique_ptr<Segment>(new LocalSegment());
    stackMap["pointer"] = std::unique_ptr<Segment>(new PointerSegment());
    stackMap["static"] = std::unique_ptr<Segment>(new StaticSegment("test.vm"));
    stackMap["temp"] = std::unique_ptr<Segment>(new TempSegment());
    stackMap["that"] = std::unique_ptr<Segment>(new ThatSegment());
    stackMap["this"] = std::unique_ptr<Segment>(new ThisSegment());

    const uint16_t IDX = 5;
    REQUIRE(test_push(stackMap, "argument", IDX).second == 0);
    REQUIRE(test_push(stackMap, "constant", IDX).second == 0);
    REQUIRE(test_push(stackMap, "local", IDX).second == 0);
    REQUIRE(test_push(stackMap, "pointer", 1).second == 0);
    REQUIRE(test_push(stackMap, "static", IDX).second == 0);
    REQUIRE(test_push(stackMap, "temp", IDX).second == 0);
    REQUIRE(test_push(stackMap, "that", IDX).second == 0);
    REQUIRE(test_push(stackMap, "this", IDX).second == 0);

    REQUIRE(test_pop(stackMap, "argument", IDX).second == 0);
    REQUIRE(test_pop(stackMap, "local", IDX).second == 0);
    REQUIRE(test_pop(stackMap, "pointer", 1).second == 0);
    REQUIRE(test_pop(stackMap, "static", IDX).second == 0);
    REQUIRE(test_pop(stackMap, "temp", IDX).second == 0);
    REQUIRE(test_pop(stackMap, "that", IDX).second == 0);
    REQUIRE(test_pop(stackMap, "this", IDX).second == 0);
}

TEST_CASE("Test relation operations", "[eq]") {
    EqGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/eq", insts).second == 0);
}
