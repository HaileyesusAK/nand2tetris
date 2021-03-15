#define CATCH_CONFIG_MAIN
#include <filesystem>
#include <cstdio>
#include <stdexcept>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "tokenizer.hpp"
#include "analyzer.hpp"
#include "utils.hpp"
#include "catch.hpp"

using namespace std;
namespace fs = filesystem;
enum class Generator {
    TERM, EXP, EXP_LIST, SYMBOL, IDENTIFIER, VAR_DEC, LET, CALL, DO, RETURN,
	WHILE, IF, PARAM, PARAM_LIST, SUB_ROUTINE_BODY, SUB_ROUTINE_DEC,
	CLASS_VAR_DEC, CLASS
};

static const fs::path EXP_DATA_DIR = DATA_DIR / "expected";

Analyzer get_analyzer(const string& cmd) {
	fs::path jackFile = DATA_DIR / "tmp.jack";
	ofstream file(jackFile);
	file << cmd;
	file.close();
	Analyzer analyzer {jackFile, 4};
	return analyzer;
}

void generateXml(Analyzer& analyzer, const Generator& generator) {
    switch(generator) {
        //case Generator::SYMBOL: analyzer.genSymbol(); break;
        case Generator::IDENTIFIER: analyzer.genIdentifier(); break;
        case Generator::TERM: analyzer.genTerm(); break;
        case Generator::EXP: analyzer.genExp(); break;
        case Generator::EXP_LIST: analyzer.genExpList(); break;
        case Generator::VAR_DEC: analyzer.genVarDec(); break;
        case Generator::LET: analyzer.genLetStatement(); break;
        case Generator::CALL: analyzer.genSubroutineCall(); break;
        case Generator::DO: analyzer.genDoStatement(); break;
        case Generator::WHILE: analyzer.genWhileStatement(); break;
        case Generator::RETURN: analyzer.genReturnStatement(); break;
        case Generator::IF: analyzer.genIfStatement(); break;
        case Generator::PARAM: analyzer.genParameter(); break;
        case Generator::PARAM_LIST: analyzer.genParameterList(); break;
        case Generator::SUB_ROUTINE_BODY: analyzer.genSubroutineBody(); break;
        case Generator::SUB_ROUTINE_DEC: analyzer.genSubroutineDec(); break;
        case Generator::CLASS: analyzer.genClass(); break;
        case Generator::CLASS_VAR_DEC: analyzer.genClassVarDec(); break;
        default: break;
    }
    analyzer.writeXml();
}

bool testFile(const fs::path& path, const Generator& generator) {
    fs::path jackFile = DATA_DIR / path;
    fs::path xmlFile(jackFile);
    xmlFile.replace_extension(".xml");
    Analyzer analyzer {jackFile, 4};
    generateXml(analyzer, generator);
    fs::path expectedFile = EXP_DATA_DIR / xmlFile.filename();
    return cmpFiles(expectedFile, xmlFile);
}

bool testCmd(const string& cmd, const string& output, const Generator& generator) {
    auto analyzer = get_analyzer(cmd);
    generateXml(analyzer, generator);
    string xmlFile("tmp.xml");
    fs::path expectedFile = EXP_DATA_DIR / xmlFile;
    ofstream file(expectedFile);
    file << output;
    file.close();
    return cmpFiles(expectedFile, DATA_DIR / xmlFile);
}

TEST_CASE("genSymbol", "[genSymbol]") {
	vector<string> symbols = {
		"{", "}", "(", ")", "[", "]", ".", ",", ";", "+", "-",
        "*", "/", "&", "|", "<", ">", "=", "_", "~"
	};

    /*
    string expectedOutput;
	for(const auto& s : symbols) {
		expectedOutput = "<symbol> " + s + " </symbol>\n";
	    REQUIRE(testCmd(s, expectedOutput, Generator::SYMBOL));
	}
    */
	auto analyzer = get_analyzer("");
	REQUIRE_THROWS_AS(analyzer.genSymbol(symbols.at(0)), out_of_range);

	analyzer = get_analyzer("{");
	REQUIRE_THROWS_AS(analyzer.genSymbol(symbols.at(1)), domain_error);

	analyzer = get_analyzer("var");
	REQUIRE_THROWS_AS(analyzer.genSymbol(symbols.at(1)), domain_error);
}

TEST_CASE("genKeyWord", "[genKeyWord]") {
	Set keywords{
		"class", "constructor", "function", "method", "field", "static",
        "var", "int", "char", "boolean", "void", "true", "false", "null",
        "this", "let", "do", "if", "else", "while", "return"
    };

	for(const auto& s : keywords) {
		auto analyzer = get_analyzer(s);
		analyzer.genKeyWord(keywords);
		analyzer.writeXml();

		fs::path expectedFile = EXP_DATA_DIR / "tmp.xml";
		ofstream file(expectedFile);
		file << "<keyword> " + s + " </keyword>" << endl;

		REQUIRE(cmpFiles(expectedFile, DATA_DIR / "tmp.xml"));
	}

	auto analyzer = get_analyzer("");  // Empty token list
	REQUIRE_THROWS_AS(analyzer.genKeyWord(keywords), out_of_range);

	analyzer = get_analyzer("varName"); //Not a keyword
	REQUIRE_THROWS_AS(analyzer.genKeyWord(keywords), domain_error);

	keywords.erase("while");    // Verify keywords are only those passed as input
	analyzer = get_analyzer("while");
	REQUIRE_THROWS_AS(analyzer.genKeyWord(keywords), domain_error);
}

TEST_CASE("Identifier", "[genIdentifier]") {
    REQUIRE(testFile("identifier.jack", Generator::IDENTIFIER));
}

TEST_CASE("Single varDec", "[genVarDec]") {
    REQUIRE(testFile("single_var_dec.jack", Generator::VAR_DEC));
}

TEST_CASE("Multi varDec", "[genVarDec]") {
    REQUIRE(testFile("multi_var_dec.jack", Generator::VAR_DEC));
}

TEST_CASE("Empty varDec", "[genVarDec]") {
    string cmd("");
	string expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::VAR_DEC), out_of_range);
}

TEST_CASE("varDec Missing var", "[genVarDec]") {
    string cmd("int age;");
	string expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::VAR_DEC), domain_error);
}

TEST_CASE("varDec Missing Semicolon", "[genVarDec]") {
    string cmd("var int age");
	string expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::VAR_DEC), out_of_range);
}

TEST_CASE("varDec Missing varName ", "[genVarDec]") {
    string cmd("var int;");
	string expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::VAR_DEC), domain_error);
}

TEST_CASE("Empty Term", "[genTerm]") {
    string cmd("");
	string expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::TERM), out_of_range);
}

TEST_CASE("Integer Constant Term", "[genTerm]") {
	REQUIRE(testFile("integerConstant.jack", Generator::TERM));
}

TEST_CASE("String Constant Term", "[genTerm]") {
	REQUIRE(testFile("stringConstant.jack", Generator::TERM));
}

TEST_CASE("Keyword Constant Term", "[genTerm]") {
	REQUIRE(testFile("keywordConstant.jack", Generator::TERM));
}

TEST_CASE("Identifier Term", "[genTerm]") {
	REQUIRE(testFile("identifier_term.jack", Generator::TERM));
}

TEST_CASE("unaryOp Term", "[genTerm]") {
	REQUIRE(testFile("unaryop_terms.jack", Generator::TERM));
}

TEST_CASE("Member Resolution Term", "[genTerm]") {
    REQUIRE(testFile("member_resolution_term.jack", Generator::TERM));
}

TEST_CASE("SubRoutine Call", "[genTerm]") {
    REQUIRE(testFile("subroutine_call.jack", Generator::TERM));
}

TEST_CASE("Parenthesized Term", "[genTerm]") {
    REQUIRE(testFile("parenthesized_term.jack", Generator::TERM));
}

TEST_CASE("Expression", "[genExp]") {
    REQUIRE(testFile("expression.jack", Generator::EXP));
}

TEST_CASE("Empty Expression List", "[genExpList]") {
    string cmd("");
	string expectedOutput = "";
	REQUIRE(testCmd(cmd, expectedOutput, Generator::EXP_LIST));
}

TEST_CASE("Multiple Expressions List", "[genExpList]") {
    REQUIRE(testFile("expression_list.jack", Generator::EXP_LIST));
}

TEST_CASE("Nested Expression", "[genExp]") {
    REQUIRE(testFile("nested_expression.jack", Generator::EXP));
}

TEST_CASE("letStatement1", "[genLetStatement]") {
    REQUIRE(testFile("let_statement1.jack", Generator::LET));
}

TEST_CASE("letStatement2", "[genLetStatement]") {
    REQUIRE(testFile("let_statement2.jack", Generator::LET));
}

TEST_CASE("Invalid letStatement", "[genLetStatement]") {
    string cmd, expectedOutput;

    cmd = "";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::LET), out_of_range);

    cmd = "age = 3;";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::LET), domain_error);

    cmd = "let = 3;";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::LET), domain_error);

    cmd = "let var 3;";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::LET), domain_error);

    cmd = "let var = 3";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::LET), domain_error);

    cmd = "let var[] = 3;";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::LET), domain_error);
}

TEST_CASE("Subroutine Call1", "[genSubroutineCall]") {
    REQUIRE(testFile("call1.jack", Generator::CALL));
}

TEST_CASE("Subroutine Call2", "[genSubroutineCall]") {
    REQUIRE(testFile("call2.jack", Generator::CALL));
}

TEST_CASE("Invalid Subroutine Call", "[genSubroutineCall]") {
    string cmd, expectedOutput;

    cmd = "";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::CALL), out_of_range);

    cmd = "while(true)";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::CALL), domain_error);

    cmd = "area(";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::CALL), out_of_range);

    cmd = "area(]";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::CALL), domain_error);

    cmd = "area.()";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::CALL), domain_error);
}

TEST_CASE("Do Statement", "[genDoStatement]") {
    REQUIRE(testFile("do.jack", Generator::DO));
}

TEST_CASE("Invalid Do Statement", "[genDoStatement]") {
    string cmd, expectedOutput;

    cmd = "";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::DO), out_of_range);

    cmd = "while area();";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::DO), domain_error);

    cmd = "do area()";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::DO), out_of_range);
}

TEST_CASE("Return Statement1", "[genReturnStatement]") {
    REQUIRE(testFile("return1.jack", Generator::RETURN));
}

TEST_CASE("Return Statement2", "[genReturnStatement]") {
    REQUIRE(testFile("return2.jack", Generator::RETURN));
}

TEST_CASE("Invalid Return Statement", "[genReturnStatement]") {
    string cmd, expectedOutput;

    cmd = "";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::RETURN), out_of_range);

    cmd = "return do;";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::RETURN), domain_error);

    cmd = "return";
	expectedOutput = "";
	REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::RETURN), out_of_range);
}

TEST_CASE("While Statement1", "[genWhileStatement]") {
    REQUIRE(testFile("while1.jack", Generator::WHILE));
}

TEST_CASE("While Statement2", "[genWhileStatement]") {
    REQUIRE(testFile("while2.jack", Generator::WHILE));
}

TEST_CASE("If Statement1", "[genIfStatement]") {
    REQUIRE(testFile("if1.jack", Generator::IF));
}

TEST_CASE("If Statement2", "[genIfStatement]") {
    REQUIRE(testFile("if2.jack", Generator::IF));
}

TEST_CASE("Subroutine Body1", "[genSubroutineBody]") {
    REQUIRE(testFile("subroutine_body1.jack", Generator::SUB_ROUTINE_BODY));
}

TEST_CASE("Subroutine Body2", "[genSubroutineBody]") {
    REQUIRE(testFile("subroutine_body2.jack", Generator::SUB_ROUTINE_BODY));
}

TEST_CASE("Subroutine Body3", "[genSubroutineBody]") {
    REQUIRE(testFile("subroutine_body3.jack", Generator::SUB_ROUTINE_BODY));
}

TEST_CASE("Subroutine Body4", "[genSubroutineBody]") {
    REQUIRE(testFile("subroutine_body4.jack", Generator::SUB_ROUTINE_BODY));
}

TEST_CASE("Parameter", "[genParameter]") {
    REQUIRE(testFile("parameter.jack", Generator::PARAM));
}

TEST_CASE("Invalid Parameter", "[genParameter]") {
	std::string cmd("int"), expectedOutput("");
    REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::PARAM), out_of_range);

	cmd = "int do";
    REQUIRE_THROWS_AS(testCmd(cmd, expectedOutput, Generator::PARAM), domain_error);
}

TEST_CASE("Parameter List1", "[genParameterList]") {
	std::string cmd(""), expectedOutput("");
    REQUIRE(testCmd(cmd, expectedOutput, Generator::PARAM_LIST));
}

TEST_CASE("Parameter List2", "[genParameterList]") {
    REQUIRE(testFile("parameter_list.jack", Generator::PARAM_LIST));
}

TEST_CASE("Subroutine Dec1", "[genSubroutineDec]") {
    REQUIRE(testFile("subroutine_dec1.jack", Generator::SUB_ROUTINE_DEC));
}

TEST_CASE("Subroutine Dec2", "[genSubroutineDec]") {
    REQUIRE(testFile("subroutine_dec2.jack", Generator::SUB_ROUTINE_DEC));
}

TEST_CASE("Subroutine Dec3", "[genSubroutineDec]") {
    REQUIRE(testFile("subroutine_dec3.jack", Generator::SUB_ROUTINE_DEC));
}

TEST_CASE("Class Var Dec1", "[genClassVarDec]") {
    REQUIRE(testFile("class_var_dec1.jack", Generator::CLASS_VAR_DEC));
}

TEST_CASE("Class Var Dec2", "[genClassVarDec]") {
    REQUIRE(testFile("class_var_dec2.jack", Generator::CLASS_VAR_DEC));
}

TEST_CASE("Class1", "[genClass]") {
    REQUIRE(testFile("class1.jack", Generator::CLASS));
}

TEST_CASE("Class2", "[genClass]") {
    REQUIRE(testFile("class2.jack", Generator::CLASS));
}

TEST_CASE("Class3", "[genClass]") {
    REQUIRE(testFile("class3.jack", Generator::CLASS));
}

TEST_CASE("Class4", "[genClass]") {
    REQUIRE(testFile("class4.jack", Generator::CLASS));
}

TEST_CASE("ArrayTest", "[genClass]") {
    REQUIRE(testFile("ArrayTest/Main.jack", Generator::CLASS));
}

TEST_CASE("ExpressionLessSquare Main", "[genClass]") {
    REQUIRE(testFile("ExpressionLessSquare/Main.jack", Generator::CLASS));
}

TEST_CASE("ExpressionLessSquare Square", "[genClass]") {
    REQUIRE(testFile("ExpressionLessSquare/Square.jack", Generator::CLASS));
}

TEST_CASE("ExpressionLessSquare SquareGame", "[genClass]") {
    REQUIRE(testFile("ExpressionLessSquare/SquareGame.jack", Generator::CLASS));
}

TEST_CASE("Square Main", "[genClass]") {
    REQUIRE(testFile("Square/Main.jack", Generator::CLASS));
}

TEST_CASE("Square Square", "[genClass]") {
    REQUIRE(testFile("Square/Square.jack", Generator::CLASS));
}

TEST_CASE("Square SquareGame", "[genClass]") {
    REQUIRE(testFile("Square/SquareGame.jack", Generator::CLASS));
}
