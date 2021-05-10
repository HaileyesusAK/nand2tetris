#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "translator.hpp"
#include "utils.hpp"
#include <CppUTest/TestHarness.h>

using namespace std;
static const fs::path DATA_DIR = TEST_DATA_DIR;

static void test(fs::path path, const AsmInst& insts) {
    saveAsm(insts, path.replace_extension(".asm"));
    std::string tstFile(path.replace_extension(".tst").string());
    auto result = execute(std::string("CPUEmulator ") + tstFile);
    cout << path << ": " << result.first << endl;
    CHECK_EQUAL(0, result.second);
}

static void test_pop(StackSegment& segment, const string& filename, uint16_t idx) {
    auto insts = segment.pop(idx);
    test(DATA_DIR / "StackOperations" / filename, insts);
}

static void test_push(StackSegment& segment, const string& filename, uint16_t idx) {
    auto insts = segment.push(idx);
    test(DATA_DIR / "StackOperations" / filename, insts);
}

TEST_GROUP(STACK_SEGMENT) {
    const uint16_t idx = 5;
};

TEST(STACK_SEGMENT, LocalSegment) {
    LocalSegment segment;
    test_push(segment, "pushlocal", idx);
    test_pop(segment, "poplocal", idx);
}

TEST(STACK_SEGMENT, ArgumentSegment) {
    ArgumentSegment segment;
    test_push(segment, "pushargument", idx);
    test_pop(segment, "popargument", idx);
}

TEST(STACK_SEGMENT, ThisSegment) {
    ThisSegment segment;
    test_push(segment, "pushthis", idx);
    test_pop(segment, "popthis", idx);
}

TEST(STACK_SEGMENT, ThatSegment) {
    ThatSegment segment;
    test_push(segment, "pushthat", idx);
    test_pop(segment, "popthat", idx);
}

TEST(STACK_SEGMENT, StaticSegment) {
    StaticSegment segment;
    segment.setFileName("test.vm");
    test_push(segment, "pushstatic", idx);
    test_pop(segment, "popstatic", idx);
}

TEST(STACK_SEGMENT, PointerSegment) {
    PointerSegment segment;
    test_push(segment, "pushpointer", 1);
    test_pop(segment, "poppointer", 1);
}

TEST(STACK_SEGMENT, TempSegment) {
    TempSegment segment;
    test_push(segment, "pushtemp", idx);
    test_pop(segment, "poptemp", idx);
}
