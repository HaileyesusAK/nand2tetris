#include <bitset>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include "assembler.hpp"
#include "utils.hpp"

#include <CppUTest/TestHarness.h>

using namespace std;

namespace fs = std::filesystem;

static const fs::path DATA_DIR = TEST_DATA_DIR;
static const fs::path EXP_DATA_DIR = DATA_DIR / "expected";

static void test_generate(const string& asmFilename) {
    auto asmFilePath = DATA_DIR / fs::path(asmFilename);
    auto expHackFile = EXP_DATA_DIR / fs::path(asmFilename);
    auto genHackFile = asmFilePath;
    genHackFile.replace_extension(".hack");
    expHackFile.replace_extension(".hack");

    Assembler assembler(asmFilePath);
    assembler.generate();
    CHECK_EQUAL(true, cmpFiles(genHackFile, expHackFile));
}

TEST_GROUP(Assembler)
{

};

TEST(Assembler, generate_add)
{
    test_generate("Add.asm");
}

TEST(Assembler, generate_max)
{
    test_generate("Max.asm");
}

TEST(Assembler, generate_mult)
{
    test_generate("Mult.asm");
}

TEST(Assembler, generate_pong)
{
    test_generate("Pong.asm");
}
