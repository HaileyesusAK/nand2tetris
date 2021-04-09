#include <filesystem>
#include <cstdio>
#include <stdexcept>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "tokenizer.hpp"
#include "code_generator.hpp"
#include "utils.hpp"
#include "catch.hpp"

using namespace std;
namespace fs = filesystem;
enum class GeneratorType {
    TERM, EXP, EXP_LIST, SYMBOL, IDENTIFIER, VAR_DEC, LET, CALL, DO, RETURN,
	WHILE, IF, PARAM, PARAM_LIST, SUB_ROUTINE_BODY, SUB_ROUTINE_DEC,
	CLASS_VAR_DEC, CLASS
};

CodeGenerator get_generator(const string& cmd) {
	fs::path jackFile = DATA_DIR / "tmp.jack";
	ofstream file(jackFile);
	file << cmd;
	file.close();
	CodeGenerator codeGenerator {jackFile};
	return codeGenerator;
}

bool testFile(const fs::path& path) {
    fs::path jackFile = DATA_DIR / path;
    fs::path vmFile(jackFile);
    vmFile.replace_extension(".vm");
    fs::path expectedFile = EXP_DATA_DIR / path;
	expectedFile.replace_extension(".vm");

	CodeGenerator codeGenerator {jackFile};
	codeGenerator.genClass();
	codeGenerator.generate();
    return cmpFiles(expectedFile, vmFile);
}

bool testCmd(const string& cmd, const string& output, const vector<GeneratorType>& instructions) {
	auto generator = get_generator(cmd);
	for(const auto& inst : instructions) {
		switch(inst) {
			case GeneratorType::TERM: generator.genTerm(); break;
			case GeneratorType::EXP: generator.genExp(); break;
			case GeneratorType::EXP_LIST: generator.genExpList(); break;
			case GeneratorType::VAR_DEC: generator.genVarDec(); break;
			case GeneratorType::LET: generator.genLetStatement(); break;
			case GeneratorType::CALL: generator.genSubroutineCall(); break;
			case GeneratorType::DO: generator.genDoStatement(); break;
			case GeneratorType::WHILE: generator.genWhileStatement(); break;
			case GeneratorType::RETURN: generator.genReturnStatement(); break;
			case GeneratorType::IF: generator.genIfStatement(); break;
			case GeneratorType::PARAM_LIST: generator.genParameterList(); break;
			case GeneratorType::SUB_ROUTINE_BODY: generator.genSubroutineBody(); break;
			case GeneratorType::SUB_ROUTINE_DEC: generator.genSubroutineDec(); break;
			case GeneratorType::CLASS: generator.genClass(); break;
			case GeneratorType::CLASS_VAR_DEC: generator.genClassVarDec(); break;
			default: break;
		}
	}
	generator.generate();
	
    fs::path vmFile = DATA_DIR / "tmp.vm";
    fs::path expectedFile = EXP_DATA_DIR / "tmp.vm";
    ofstream file(expectedFile);
	file << output << endl;
    file.close();

    return cmpFiles(expectedFile, vmFile);
}

TEST_CASE("Assign a constant to a local variable", "[genLetStatement]") {
    // tests also varDec
    string cmd {"var int x, y; var int z; let z = 5;"};
	vector<GeneratorType> instructions {
        GeneratorType::VAR_DEC, GeneratorType::VAR_DEC, GeneratorType::LET
    };
	string output {"push constant 5\npop local 2"};
	REQUIRE(testCmd(cmd, output, instructions)); 

}

TEST_CASE("Assign an expression to an argument", "[genLetStatement]") {
    // tests also varDec and parameterList
    string cmd {"(int x, int y) var int z; let y = -(x + z);"};
	vector<GeneratorType> instructions {
        GeneratorType::PARAM_LIST, GeneratorType::VAR_DEC, GeneratorType::LET
    };
	string output {"push argument 0\npush local 0\nadd\nneg\npop argument 1"};
	REQUIRE(testCmd(cmd, output, instructions)); 
}

TEST_CASE("Array assignment") {
    string cmd {"(int x, int y) var int a, i; let a[i + 5] = ~(x + y);"};
	vector<GeneratorType> instructions {
        GeneratorType::PARAM_LIST, GeneratorType::VAR_DEC, GeneratorType::LET
    };
	string output {"push local 1\npush constant 5\nadd\n"
                   "push local 0\nadd\npop pointer 1\n"
                   "push argument 0\npush argument 1\nadd\nnot\n"
                   "pop that 0"};
	REQUIRE(testCmd(cmd, output, instructions)); 
}

TEST_CASE("Subroutine call", "[genSubroutineCall]") {
    string cmd {"do Circle.area(1 + (2 * 3));"};
	vector<GeneratorType> instructions { GeneratorType::DO };
	string output {"push constant 1\npush constant 2\npush constant 3\n"
				   "call Math.multiply 2\nadd\ncall Circle.area 1\npop temp 0"};
	REQUIRE(testCmd(cmd, output, instructions)); 
}

TEST_CASE("Seven") {
    REQUIRE(testFile(fs::path("Seven") / fs::path("Main.jack")));
}

TEST_CASE("ConvertToBin") {
    REQUIRE(testFile(fs::path("ConvertToBin") / fs::path("Main.jack")));
}

TEST_CASE("Average") {
    REQUIRE(testFile(fs::path("Average") / fs::path("Main.jack")));
}

