#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "generator.hpp"
#include "translator.hpp"
#include "utils.hpp"
#include "catch.hpp"

using namespace std;

namespace fs = std::filesystem;
static const fs::path DATA_DIR = fs::current_path().parent_path() / "data";

static void test_vm_translator(VmTranslator& translator, fs::path path) {
    translator.translate(path);
    if(fs::is_directory(path))
        path = path / path.filename();
    std::string tstFile(path.replace_extension(".tst").string());
    auto result = execute(std::string("CPUEmulator ") + tstFile);
    INFO (result.first);
    REQUIRE(result.second == 0);
}

TEST_CASE("Translate a File", "[Single File]") {
    VmTranslator translator;
    test_vm_translator(translator, DATA_DIR / "StackTest/StackTest.vm");
    test_vm_translator(translator, DATA_DIR / "BasicTest/BasicTest.vm");
    test_vm_translator(translator, DATA_DIR / "PointerTest/PointerTest.vm");
    test_vm_translator(translator, DATA_DIR / "SimpleAdd/SimpleAdd.vm");
    test_vm_translator(translator, DATA_DIR / "StaticTest/StaticTest.vm");
    test_vm_translator(translator, DATA_DIR / "ProgramFlow/BasicLoop/BasicLoop.vm");
    test_vm_translator(translator, DATA_DIR / "ProgramFlow/FibonacciSeries/FibonacciSeries.vm");
    test_vm_translator(translator, DATA_DIR / "FunctionCalls/SimpleFunction/SimpleFunction.vm");
}

TEST_CASE("Translate a Directory", "[Single Directory]") {
    VmTranslator translator;
    test_vm_translator(translator, DATA_DIR / "FunctionCalls/FibonacciElement");
    test_vm_translator(translator, DATA_DIR / "FunctionCalls/NestedCall");
}