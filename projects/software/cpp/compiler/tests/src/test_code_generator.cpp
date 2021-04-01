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

TEST_CASE("Integer Constant Term Generation", "[genTerm]") {
    string cmd {"var int x;\nlet x = 5;"};
	vector<GeneratorType> instructions { GeneratorType::VAR_DEC, GeneratorType::LET };
	string output {"push constant 5\npop local 0"};
	REQUIRE(testCmd(cmd, output, instructions)); 
}
