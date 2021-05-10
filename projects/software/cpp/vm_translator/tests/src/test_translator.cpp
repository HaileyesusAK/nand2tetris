#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "translator.hpp"
#include "utils.hpp"
#include <CppUTest/TestHarness.h>

using namespace std;
static const fs::path DATA_DIR = TEST_DATA_DIR;

static void test(Translator& translator, fs::path path) {
    translator.translate(path);
    if(fs::is_directory(path))
        path = path / path.filename();

    path.replace_extension(".tst");
    std::string tstFile(path.string());
    auto result = execute(std::string("CPUEmulator ") + tstFile);
    cout << path << ": " << result.first << endl;
    CHECK_EQUAL(0, result.second);
}

TEST_GROUP(TRANSLATOR) {
    Translator translator;
};

TEST(TRANSLATOR, STACK_TEST) {
    test(translator, DATA_DIR / "StackTest/StackTest.vm");
}

TEST(TRANSLATOR, BASIC_TEST) {
    test(translator, DATA_DIR / "BasicTest/BasicTest.vm");
}

TEST(TRANSLATOR, POINTER_TEST) {
    test(translator, DATA_DIR / "PointerTest/PointerTest.vm");
}

TEST(TRANSLATOR, SIMPLE_ADD) {
    test(translator, DATA_DIR / "SimpleAdd/SimpleAdd.vm");
}

TEST(TRANSLATOR, STATIC_TEST) {
    test(translator, DATA_DIR / "StaticTest/StaticTest.vm");
}

TEST(TRANSLATOR, BASIC_LOOP) {
    test(translator, DATA_DIR / "ProgramFlow/BasicLoop/BasicLoop.vm");
}

TEST(TRANSLATOR, FibonacciElement) {
    test(translator, DATA_DIR / "FunctionCalls/FibonacciElement");
}

TEST(TRANSLATOR, NestedCall) {
    test(translator, DATA_DIR / "FunctionCalls/NestedCall");
}

TEST(TRANSLATOR, StaticsTest) {
    test(translator, DATA_DIR / "FunctionCalls/StaticsTest");
}

/*
//TODO: Figure out why CppUTest is reporting a memory leak.
//Moving the files one level up the directory hierarchy doesn't produce the error,
//It appears the length of the path has got something to do with it

TEST(TRANSLATOR, SIMPLE_FUNCTION) {
    test(translator, DATA_DIR / "FunctionCalls/SimpleFunction/SimpleFunction.vm");
}

TEST(TRANSLATOR, FIBONACCI_SERIES) {
    test(translator, DATA_DIR / "ProgramFlow/FibonacciSeries/FibonacciSeries.vm");
}
*/
