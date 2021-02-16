#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <filesystem>
#include <regex>
#include <unordered_set>
#include <unordered_map>
#include "tokenizer.hpp"

using Set = std::unordered_set<std::string>;

Tokenizer::Tokenizer(const fs::path& jackPath) {
    std::ifstream file(jackPath);
    if(!file.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + jackPath.string());

    tokenize(file);
    it = tokens.begin();
}

bool Tokenizer::hasNext() { return it != tokens.end(); }
Token Tokenizer::getNext() { return *it++; }

void Tokenizer::tokenize(std::ifstream& file) {
    std::unordered_map<std::string, TokenType> symbolMap {
        {"{", TokenType::LEFT_BRACE},
        {"}", TokenType::RIGHT_BRACE},
        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {"[", TokenType::LEFT_BRACKET},
        {"]", TokenType::RIGHT_BRACKET},
        {".", TokenType::DOT},
        {",", TokenType::COMMA},
        {";", TokenType::SEMICOLON},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},
        {"*", TokenType::MULT},
        {"/", TokenType::DIV},
        {"&", TokenType::AND},
        {"|", TokenType::BAR},
        {"<", TokenType::LEFT_ANGLE_BRACKET},
        {">", TokenType::RIGHT_ANGLE_BRACKET},
        {"=", TokenType::EQUAL},
        {"_", TokenType::UNDER_SCORE}
    };

    Set keywords {"class", "constructor", "function", "method", "field", "static", "var",
        "int", "char", "boolean", "void", "true", "false", "null", "this", "let", "do",
        "if", "else", "while", "return"
    };

    std::string tokenPattern {"(\"[^\"]*\")|(\\w+)"};

    // These symbols needs to be escaped since they have special meaning in std::regex
    Set escapedSybmols {"{", "}", "(", ")", "[", "]", ".", "+", "*", "|"};
    for(const auto& symbol : escapedSybmols)
        tokenPattern += "|(\\" + symbol + ")";

    for(const auto& symbol : symbolMap) {
        if(escapedSybmols.count(symbol.first))
            continue;

        tokenPattern += "|(" + symbol.first + ")";
    }
    tokenPattern += "|([^\\s*]+?)";

    std::regex tokenRegex(tokenPattern);
    std::sregex_iterator end;
    Token token;
    std::string line;
    uint32_t lineNo = 0;

    auto isKeyword = [&keywords](const std::string& s) {return keywords.count(s);};
    auto isSymbol = [&symbolMap](const std::string& s) {return symbolMap.count(s);};
    auto isString = [](const std::string& s) {return std::regex_match(s, std::regex("\"[^\"]*\""));};
    auto isIdentifier = [](const std::string& s){
        return std::regex_match(s, std::regex("[a-zA-Z_]\\w*"));
    };
    auto isInteger = [](const std::string& s) {
        auto m = std::regex_match(s, std::regex("\\d{1,5}"));
        return m && std::stoul(s) < 32767;
    };

    while(std::getline(file, line)) {
        ++lineNo;
        std::sregex_iterator pos {line.begin(), line.end(), tokenRegex};
        while(pos != end) {
            auto value = pos->str();
            token.value = value;
            token.lineNo = lineNo;
            token.columnNo = pos->position();

            if(isKeyword(value))
                token.type= TokenType::KEYWORD;
            else if(isSymbol(value))
                token.type = symbolMap[value];
            else if(isString(value))
                token.type = TokenType::STRING;
            else if(isIdentifier(value))
                token.type = TokenType::IDENTIFIER;
            else if(isInteger(value))
                token.type = TokenType::INTEGER;
            else
                token.type = TokenType::UNKNOWN;

            tokens.push_back(token);
            pos++;
        }
    }
}

void Tokenizer::writeOpeningTag(const std::string& tag, std::ostream& output_stream) 
{
    output_stream << "<" << tag << ">";
}

void Tokenizer::writeClosingTag(const std::string& tag, std::ostream& output_stream) 
{
    output_stream << "</" << tag << ">";
}

void Tokenizer::writeXml(const std::string& tag, const std::string& text, std::ostream& output_stream)
{
    writeOpeningTag(tag, output_stream);
    output_stream << " " << text << " ";
    writeClosingTag(tag, output_stream);
    output_stream << "\n";
}

void Tokenizer::writeXml(std::ostream& output_stream) {
    std::string tag, text;
    writeOpeningTag("tokens", output_stream); 
    output_stream << "\n";

    while(hasNext()) {
        Token token = getNext();
        text = token.value;
        if(token.type == TokenType::STRING) {
            // Remove the quotes before generating the xml
            text.erase(0,1);
            text.pop_back();
        }

        switch(token.type) {
            case TokenType::INTEGER:
                tag ="integerConstant";
            	break;

            case TokenType::STRING:
                tag ="stringConstant";
            	break;

            case TokenType::KEYWORD:
                tag ="keyword";
            	break;

            case TokenType::IDENTIFIER:
                tag ="identifier";
            	break;

            case TokenType::UNKNOWN:
                tag ="unknown";
            	break;

            default:
                tag ="symbol";
            	break;
        };
        writeXml(tag, text, output_stream);
    }

    writeClosingTag("tokens", output_stream); 
    it = tokens.begin();
}
