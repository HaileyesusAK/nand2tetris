#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <unordered_set>

namespace fs = std::filesystem;

enum class TokenType {IDENTIFIER, INTEGER, KEYWORD, SYMBOL, STRING, UNKNOWN};

struct Token {
    TokenType type;
    std::string value;
    uint32_t lineNo;
    uint32_t columnNo;
	friend bool operator==(const Token& lhs, const Token& rhs) {
		return lhs.type == rhs.type && lhs.value == rhs.value;
	}

	friend std::ostream& operator<<(std::ostream& os, const Token& token) {
		os << "'" << token.value << "' at (" << token.lineNo << ", " << token.columnNo << ")";
		return os;
	}
};

using Set = std::unordered_set<std::string>;

class Tokenizer {
    std::vector<Token> tokens;
    std::vector<Token>::iterator it;
    void tokenize(std::ifstream&);
	std::vector<std::string> inputLines;
	fs::path outputPath;
    static TokenType getTokenType(const std::string& token);
    static const Set& getSymbols();
    static const Set& getKeyWords();

    public:
	Tokenizer(){}
    Tokenizer(const fs::path& jackPath);
	Token getNext() { return *it++; }
	void putBack() { --it; }
	bool hasNext() { return it != tokens.end(); }
	std::string getLine(size_t lineNo);
	void generateXml();
	static std::string convertXmlSymbol(const std::string& symbol);
};

#endif
