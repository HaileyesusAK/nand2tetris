#define CATCH_CONFIG_MAIN
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "generator.hpp"
#include "utils.hpp"
#include "catch.hpp"

using namespace std;
namespace fs = std::filesystem;

static const fs::path DATA_DIR = fs::current_path().parent_path() / "data";

std::pair<std::string, int> test(fs::path path, const AsmInst& insts) {
    saveAsm(insts, path.replace_extension(".asm"));
    std::string tstFile(path.replace_extension(".tst").string());
    return execute(std::string("CPUEmulator ") + tstFile);
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
    stackMap["static"] = std::shared_ptr<Segment>(new StaticSegment());
    stackMap["temp"] = std::shared_ptr<Segment>(new TempSegment());
    stackMap["that"] = std::shared_ptr<Segment>(new ThatSegment());
    stackMap["this"] = std::shared_ptr<Segment>(new ThisSegment());

    auto staticSegment = std::static_pointer_cast<StaticSegment>(stackMap["static"]);
    staticSegment->setFileName("test.vm");

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
    REQUIRE(test(DATA_DIR / "eq", insts).second == 0);
}

TEST_CASE("Test LtGenerator", "[lt]") {
    LtGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "lt", insts).second == 0);
}

TEST_CASE("Test GtGenerator", "[gt]") {
    GtGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "gt", insts).second == 0);
}

TEST_CASE("Test NegGenerator", "[neg]") {
    NegGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "neg", insts).second == 0);
}

TEST_CASE("Test NotGenerator", "[not]") {
    NotGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "not", insts).second == 0);
}

TEST_CASE("Test AndGenerator", "[and]") {
    AndGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "and", insts).second == 0);
}

TEST_CASE("Test OrGenerator", "[or]") {
    OrGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "or", insts).second == 0);
}

TEST_CASE("Test AddGenerator", "[add]") {
    AddGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "add", insts).second == 0);
}

TEST_CASE("Test SubGenerator", "[sub]") {
    SubGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "sub", insts).second == 0);
}

TEST_CASE("Test FunctionGenerator", "[function]") {
    FunctionGenerator generator;
    auto insts = generator.generate("fibo", 2);
    REQUIRE(test(DATA_DIR / "func", insts).second == 0);
}

TEST_CASE("Test CallGenerator", "[call]") {
    CallGenerator generator;
    auto insts = generator.generate("fibo", 2);
    REQUIRE(test(DATA_DIR / "call", insts).second == 0);
}

TEST_CASE("Test ReturnGenerator", "[ret]") {
    ReturnGenerator generator;
    auto insts = generator.generate();
    REQUIRE(test(DATA_DIR / "return", insts).second == 0);
}
