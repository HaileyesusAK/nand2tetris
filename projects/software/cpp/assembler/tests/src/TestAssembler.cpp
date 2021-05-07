#include <bitset>
#include <fstream>
#include <functional>
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

TEST(Assembler, directory_assembly)
{
    using namespace std::placeholders;

    auto is_type = [](const fs::path& p, const std::string& ext) {
        return (!fs::is_directory(p)) && p.extension() == ext;
    };

    //Remove existing hack files
    auto is_hack = std::bind(is_type, _1, ".hack");
    std::for_each(fs::directory_iterator(DATA_DIR),
                  fs::directory_iterator(),
                  [&is_hack](const auto& p){ if(is_hack(p)) fs::remove(p);});

    auto assembler = Assembler(DATA_DIR);
    assembler.generate();

    auto is_asm = std::bind(is_type, _1, ".asm");
    auto n_asm = std::count_if(fs::directory_iterator(DATA_DIR), fs::directory_iterator(), is_asm);
    auto n_hack = std::count_if(fs::directory_iterator(DATA_DIR), fs::directory_iterator(), is_hack);

    //The number of generated hack files should be equal to the number of assembly files
    CHECK_EQUAL(n_asm, n_hack);
}
