#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "translator.hpp"
#include "utils.hpp"
#include <CppUTest/TestHarness.h>

using namespace std;
static const fs::path DATA_DIR = TEST_DATA_DIR;
static void execute_tst(fs::path& path) {
    path.replace_extension(".tst");
    std::string tstFile(path.string());
    auto result = execute(std::string("CPUEmulator ") + tstFile);
    cout << path << ": " << result.first << endl;
    CHECK_EQUAL(0, result.second);
}

static void test(Translator& translator, void(Translator::*translatorFunc)(), fs::path path) {
    translator.setOutputFile(path);
    (translator.*translatorFunc)(); 
    execute_tst(path);
}

/*
static void test(Translator& translator, void(Translator::*translatorFunc)(const std::string&), const std::string& arg1, fs::path path) {
    translator.setOutputFile(path);
    (translator.*translatorFunc)(arg1); 
    execute_tst(path);
}
*/

static void test(Translator& translator, void(Translator::*translatorFunc)(const std::string&, uint16_t), const std::string& arg1, uint16_t arg2, fs::path path) {
    translator.setOutputFile(path);
    (translator.*translatorFunc)(arg1, arg2); 
    execute_tst(path);
}

TEST_GROUP(SINGLE_INSTRUCTION) {
    Translator translator;
};

TEST(SINGLE_INSTRUCTION, Add) {
    test(translator, &Translator::translateAdd, DATA_DIR / "Others/add.vm");
}

TEST(SINGLE_INSTRUCTION, Sub) {
    test(translator, &Translator::translateSub, DATA_DIR / "Others/sub.vm");
}

TEST(SINGLE_INSTRUCTION, Eq) {
    test(translator, &Translator::translateEq, DATA_DIR / "Others/eq.vm");
}

TEST(SINGLE_INSTRUCTION, Gt) {
    test(translator, &Translator::translateGt, DATA_DIR / "Others/gt.vm");
}

TEST(SINGLE_INSTRUCTION, Lt) {
    test(translator, &Translator::translateLt, DATA_DIR / "Others/lt.vm");
}

TEST(SINGLE_INSTRUCTION, Neg) {
    test(translator, &Translator::translateNeg, DATA_DIR / "Others/neg.vm");
}

TEST(SINGLE_INSTRUCTION, Not) {
    test(translator, &Translator::translateNot, DATA_DIR / "Others/not.vm");
}

TEST(SINGLE_INSTRUCTION, Or) {
    test(translator, &Translator::translateOr, DATA_DIR / "Others/or.vm");
}

TEST(SINGLE_INSTRUCTION, And) {
    test(translator, &Translator::translateAnd, DATA_DIR / "Others/and.vm");
}

TEST(SINGLE_INSTRUCTION, Function) {
    test(translator, &Translator::translateFunction, "fibo", 2, DATA_DIR / "Others/func.vm");
}

TEST(SINGLE_INSTRUCTION, Call) {
    test(translator, &Translator::translateCall, "fibo", 2, DATA_DIR / "Others/call.vm");
}

TEST(SINGLE_INSTRUCTION, Return) {
    test(translator, &Translator::translateReturn, DATA_DIR / "Others/return.vm");
}

/*
TEST(SINGLE_INSTRUCTION, Goto) {
    translator.setCurrentFunctionName("fibo");
    test(translator, &Translator::translateGoto, "EXIT", DATA_DIR / "Others/goto.vm");
}

TEST(SINGLE_INSTRUCTION, IfGoto) {
    translator.setCurrentFunctionName("fibo");
    test(translator, &Translator::translateGoto, "EXIT", DATA_DIR / "Others/ifgoto.vm");
}

TEST(SINGLE_INSTRUCTION, Label) {
    translator.setCurrentFunctionName("fibo");
    test(translator, &Translator::translateGoto, "EXIT", DATA_DIR / "Others/goto.vm");
}
*/
