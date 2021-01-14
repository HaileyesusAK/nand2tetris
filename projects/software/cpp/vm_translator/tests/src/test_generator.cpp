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
    auto insts = stackMap.at(segment)->push(idx);
    string filename(string("../data/push") + segment);
    return test(filename, insts);
}

static pair<string, int> test_pop(const StackCodeMap& stackMap, const string& segment, uint16_t idx) {
    auto insts = stackMap.at(segment)->pop(idx);
    string filename(string("../data/pop") + segment);
    return test(filename, insts);
}

TEST_CASE("Test stack operations", "[stack]") {

    StackCodeMap stackMap;
    stackMap["argument"] = std::shared_ptr<Segment>(new ArgumentSegment());
    stackMap["constant"] = std::shared_ptr<Segment>(new ConstantSegment());
    stackMap["local"] = std::shared_ptr<Segment>(new LocalSegment());
    stackMap["pointer"] = std::shared_ptr<Segment>(new PointerSegment());
    stackMap["static"] = std::shared_ptr<Segment>(new StaticSegment("test.vm"));
    stackMap["temp"] = std::shared_ptr<Segment>(new TempSegment());
    stackMap["that"] = std::shared_ptr<Segment>(new ThatSegment());
    stackMap["this"] = std::shared_ptr<Segment>(new ThisSegment());


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

TEST_CASE("Test EqGenerator", "[eq]") {
    EqGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/eq", insts).second == 0);
}

TEST_CASE("Test LtGenerator", "[lt]") {
    LtGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/lt", insts).second == 0);
}

TEST_CASE("Test GtGenerator", "[gt]") {
    GtGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/gt", insts).second == 0);
}

TEST_CASE("Test NegGenerator", "[neg]") {
    NegGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/neg", insts).second == 0);
}

TEST_CASE("Test NotGenerator", "[not]") {
    NotGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/not", insts).second == 0);
}

TEST_CASE("Test AndGenerator", "[and]") {
    AndGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/and", insts).second == 0);
}

TEST_CASE("Test OrGenerator", "[or]") {
    OrGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/or", insts).second == 0);
}

TEST_CASE("Test AddGenerator", "[add]") {
    AddGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/add", insts).second == 0);
}

TEST_CASE("Test SubGenerator", "[sub]") {
    SubGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/sub", insts).second == 0);
}

TEST_CASE("Test FunctionGenerator", "[function]") {
    FunctionGenerator generator;
    auto insts = generator.generate("fibo", 2);
    REQUIRE(test("../data/func", insts).second == 0);
}

TEST_CASE("Test CallGenerator", "[call]") {
    CallGenerator generator;
    auto insts = generator.generate("fibo", 2);
    REQUIRE(test("../data/call", insts).second == 0);
}

TEST_CASE("Test RetGenerator", "[ret]") {
    RetGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test("../data/ret", insts).second == 0);
}
