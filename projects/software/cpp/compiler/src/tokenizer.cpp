#include <cerrno>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <regex>
#include <unordered_set>
#include "tokenizer.hpp"


Tokenizer::Tokenizer(const fs::path& jackPath) {
    std::ifstream file(jackPath);
    if(!file.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + jackPath.string());

    tokenize(file);
    it = tokens.begin();
}

const Set& Tokenizer::getSymbols() {
    static Set symbols {"{", "}", "(", ")", "[", "]", ".", ",", ";", "+", "-",
						"*", "/", "&", "|", "<", ">", "=", "_", "~"};
    return symbols;
}

const Set& Tokenizer::getKeyWords() {
    static Set keywords {"class", "constructor", "function", "method", "field", "static",
        "var", "int", "char", "boolean", "void", "true", "false", "null", "this", "let",
        "do", "if", "else", "while", "return"
    };

    return keywords;
}

TokenType Tokenizer::getTokenType(const std::string& token) {
    TokenType tokenType;
    static Set symbols = Tokenizer::getSymbols();
    static Set keywords = Tokenizer::getKeyWords();

    static auto isKeyword = [](const std::string& s) {
        return keywords.count(s);
    };

    static auto isSymbol = [](const std::string& s) {
        return symbols.count(s);
    };

    static auto isString = [](const std::string& s) {
        return std::regex_match(s, std::regex("\"[^\"]*\""));
    };

    static auto isIdentifier = [](const std::string& s){
        return std::regex_match(s, std::regex("[a-zA-Z_]\\w*"));
    };

    static auto isInteger = [](const std::string& s) {
        auto m = std::regex_match(s, std::regex("\\d{1,5}"));
        return m && std::stoul(s) < 32767;
    };

    if(isKeyword(token))
        tokenType= TokenType::KEYWORD;
    else if(isSymbol(token))
        tokenType = TokenType::SYMBOL;
    else if(isString(token))
        tokenType = TokenType::STRING;
    else if(isIdentifier(token))
        tokenType = TokenType::IDENTIFIER;
    else if(isInteger(token))
        tokenType = TokenType::INTEGER;
    else
        tokenType = TokenType::UNKNOWN;

    return tokenType;
}

void Tokenizer::tokenize(std::ifstream& file) {
    Set symbols = Tokenizer::getSymbols();
    Set keywords = Tokenizer::getKeyWords();
    std::string tokenPattern {"(\"[^\"]*\")|(\\w+)"};

    // These symbols need to be escaped since they have special meaning in std::regex
    Set escapedSybmols {"{", "}", "(", ")", "[", "]", ".", "+", "*", "|"};
    for(const auto& symbol : escapedSybmols)
        tokenPattern += "|(\\" + symbol + ")";

    for(const auto& symbol : symbols) {
        if(escapedSybmols.count(symbol))
            continue;

        tokenPattern += "|(" + symbol + ")";
    }
    tokenPattern += "|([^\\s*]+?)";

    std::regex tokenRegex(tokenPattern);
    std::sregex_iterator end;
    Token token;
	std::string prevTokenVal;
    std::string line;
    uint32_t lineNo = 0;
    bool underCommentSec = false;

    while(std::getline(file, line)) {
		inputLines.push_back(line);
        ++lineNo;
        std::sregex_iterator pos {line.begin(), line.end(), tokenRegex};

        prevTokenVal = "";
        while(pos != end) {
            auto value = pos->str();
            token.value = value;
            token.lineNo = lineNo;
            token.columnNo = pos->position() + 1;
            token.type = getTokenType(value);

            if(!underCommentSec) {
				if(token.value == "/" && prevTokenVal == "/") {
                    //A line comment
                    tokens.pop_back();
                    break; //Ignore the rest of the line
				}
				else if(token.value == "*" && prevTokenVal == "/") {
                    //Entering a multiline comment section
                    tokens.pop_back();
                    underCommentSec = true;
				}
				else {
                    tokens.push_back(token);
				}
            }
			else if(token.value == "/" && prevTokenVal == "*") {
                //Closing a multiline comment section
                underCommentSec = false;
            }

            pos++;
            prevTokenVal = token.value;
        }
    }
}

std::string Tokenizer::getLine(size_t lineNo) { return inputLines.at(lineNo); }
