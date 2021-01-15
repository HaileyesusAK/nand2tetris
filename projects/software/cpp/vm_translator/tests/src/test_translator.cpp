#include <fstream>
#include <iostream>
#include <string>
#include "generator.hpp"
#include "translator.hpp"
#include "utils.hpp"
#include "catch.hpp"

using namespace std;

static void test_vm_translator(VmTranslator& translator, const string& path) {
    auto insts = translator.translate(path + ".vm");
    REQUIRE(test(path, insts).second == 0);
}

TEST_CASE("Test VmTranslator", "[Single File]") {
    VmTranslator translator;
    test_vm_translator(translator, "../data/StackTest/StackTest");
    test_vm_translator(translator, "../data/BasicTest/BasicTest");
    test_vm_translator(translator, "../data/PointerTest/PointerTest");
    test_vm_translator(translator, "../data/SimpleAdd/SimpleAdd");
    test_vm_translator(translator, "../data/StaticTest/StaticTest");
    //test_vm_translator(translator, "../data/FunctionCalls/SimpleFunction/SimpleFunction");
}

