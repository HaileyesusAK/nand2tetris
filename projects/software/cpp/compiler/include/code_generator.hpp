#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>

#include "tokenizer.hpp"

namespace fs = std::filesystem;

struct Symbol {
	std::string name;
	std::string type;
	std::string kind;
	uint16_t index;
};

using SymbolTable = std::unordered_map<std::string, Symbol>;

class CodeGenerator {
	fs::path inputPath;
	std::vector<std::string> output;
	Tokenizer tokenizer;
	SymbolTable classSymbols;
	SymbolTable subroutineSymbols;
	uint16_t numClassVars = 0;
	uint16_t numSubroutineVars = 0;

	const Symbol& resolveSymbol(const std::string& name);
	std::string getBinOpInst(const std::string& op);
	void genVarDecList(SymbolTable& symbolTable, const std::string& kind, uint16_t& index);
	void appendInputLine(std::string& s, size_t lineNo, size_t columnNo);

	public:
	CodeGenerator(const fs::path& inputPath);
	void generate();
	void genDoStatement();
	void genClass();
	void genClassVarDec();
	void genIfStatement();
	void genExp();
	uint16_t genExpList();
	void genLetStatement();
	void genParameterList();
	void genReturnStatement();
	void genTerm();
	void genStatements();
	void genSubroutineBody();
	void genSubroutineDec();
	void genSubroutineCall();
	void genVarDec();
	void genWhileStatement();
	Token getIdentifier();
	Token getSymbol(const std::string& symbol);
	Token getType();
	Token getKeyWord(const Set& keywords);
};

#endif
