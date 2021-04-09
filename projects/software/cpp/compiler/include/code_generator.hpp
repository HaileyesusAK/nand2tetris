#ifndef __CODE_GENERATOR_H__
#define __CODE_GENERATOR_H__

#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>

#include "heap.hpp"
#include "symbol_table.hpp"
#include "tokenizer.hpp"
#include "vm_writer.hpp"

namespace fs = std::filesystem;

enum class SubroutineType {
	CONSTRUCTOR,
	FUNCTION,
	METHOD
};

class CodeGenerator {
    std::string className;
	Tokenizer tokenizer;
	SubroutineType currentSubroutineType;
	std::string currentSubroutineName;
    SymbolTable symbolTable;
    VmWriter vmWriter;

	Command getArithCmd(const std::string& op);
	uint16_t genVarDecList(const SymbolKind& kind);
	void appendInputLine(std::string& s, size_t lineNo, size_t columnNo);
    Segment kindToSegment(const SymbolKind& kind);
	void report(const std::string& caller);

	public:
	CodeGenerator(const fs::path& inputPath);
	void generate();
	void genDoStatement();
	void genClass();
	uint16_t genClassVarDec();
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
	uint16_t genVarDec();
	void genWhileStatement();
	Token getIdentifier();
	Token getSymbol(const std::string& symbol);
	Token getType();
	Token getKeyWord(const Set& keywords);
};

#endif
