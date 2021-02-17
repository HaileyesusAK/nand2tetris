#ifndef __ANALYZER_H__
#define __ANALYZER_H__

#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "tokenizer.hpp"


namespace fs = std::filesystem;
using TokenTag = std::unordered_map<std::string, std::string>;

class Analyzer {
	fs::path inputPath;
	size_t level = 0;
	std::string output;
	size_t tabWidth = 4;
	Tokenizer tokenizer;
	size_t printLine(const std::string& line);
	std::string convertXmlSymbol(const std::string& symbol);
    void rewind(size_t pos);

	public:
	Analyzer(){}
	Analyzer(const fs::path& inputPath, size_t tabWidth);
	void genClass();
	void genClassVarDec();
	void genDoStatement();
	void genExp();
	void genExpList();
	void genIdentifier();
	void genIfStatement();
	void genKeyWord(const Set& keywords);
	void genLetStatement();
	void genParameter();
	void genParameterList();
	void genReturnStatement();
	void genStatements();
	void genSubroutineBody();
	void genSubroutineCall();
	void genSubroutineDec();
	void genSymbol(const std::string& symbol);
	void genTerm();
	void genType();
	void genVarDec();
	void genVarDecList();
	void genWhileStatement();
	void writeXml();
};

#endif
