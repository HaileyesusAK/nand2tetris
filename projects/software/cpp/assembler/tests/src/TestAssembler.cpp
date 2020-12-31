#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "assembler.hpp"

#include <CppUTest/TestHarness.h>

using namespace std;

static SimpleString StringFrom(const AsmInstType& type){
    switch(type) {
        case AsmInstType::Label:
            return SimpleString("Label");
        case AsmInstType::Blank:
            return SimpleString("Blank");
        case AsmInstType::AInst:
            return SimpleString("AInst");
        default:
            return SimpleString("CInst");
    }
}

static void test_generate(const string& asmFilePath, const string& hackFilePath) {
    auto asmFile = ifstream(asmFilePath);
    auto hackFile = ifstream(hackFilePath);
    Assembler assembler(asmFile);

    auto actualCode = assembler.generate();
    auto it = actualCode.begin(); 
    string line;
    while(getline(hackFile, line)) {
        CHECK_EQUAL(line, (*it).to_string());
        ++it;
    }
}

TEST_GROUP(Assembler)
{

};

TEST(Assembler, compact)
{
    CHECK_EQUAL(string("ADM=D;JEQ"), Assembler::compact(string(" ADM = D; JEQ //Comment")));
}

TEST(Assembler, extractInst_Label)
{
    string s("(LABEL)");
    auto inst = Assembler::extractInst(s);
    CHECK_EQUAL(AsmInstType::Label, inst.type);
    CHECK_EQUAL(string("LABEL"), inst.str);
}

TEST(Assembler, extractInst_comment)
{
    string s("//blbla");
    auto inst = Assembler::extractInst(s);
    CHECK_EQUAL(AsmInstType::Blank, inst.type);
}

TEST(Assembler, extractInst_empty)
{
    string s(" ");
    auto inst = Assembler::extractInst(s);
    CHECK_EQUAL(AsmInstType::Blank, inst.type);
}

TEST(Assembler, extractInst_AInst)
{
    string s("@LABEL");
    auto inst = Assembler::extractInst(s);
    CHECK_EQUAL(AsmInstType::AInst, inst.type);
    CHECK_EQUAL(string("LABEL"), inst.str);
}

TEST(Assembler, extractInst_CInst)
{
    string s("M=D;JGT");
    auto inst = Assembler::extractInst(s);
    CHECK_EQUAL(AsmInstType::CInst, inst.type);
    CHECK_EQUAL(s, inst.str);
}

TEST(Assembler, generate_dst_cmp_jmp)
{
    string c_inst("AMD=D+A;JLT");
    CHECK_EQUAL(57532, Assembler::generate(c_inst));
}

TEST(Assembler, generate_dst_cmp)
{
    string c_inst("D=-M");
    CHECK_EQUAL(64720, Assembler::generate(c_inst));
}

TEST(Assembler, generate_cmp)
{
    string c_inst("D&A");
    CHECK_EQUAL(57344, Assembler::generate(c_inst));
}

TEST(Assembler, generate_mult)
{
    test_generate("../data/Mult.asm", "../data/Mult.hack");
}

TEST(Assembler, generate_pong)
{
    test_generate("../data/Pong.asm", "../data/Pong.hack");
}
