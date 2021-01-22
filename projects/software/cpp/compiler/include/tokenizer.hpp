#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

enum class TokenType {
    KEYWORD, LEFT_BRACE, RIGHT_BRACE, LEFT_PAREN, RIGHT_PAREN, LEFT_BRACKET, RIGHT_BRACKET,
    DOT, COMMA, SEMICOLON, PLUS, MINUS, MULT, DIV, AND, BAR, LEFT_ANGLE_BRACKET,
    RIGHT_ANGLE_BRACKET, EQUAL, UNDER_SCORE, STRING, INTEGER, IDENTIFIER, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    uint32_t lineNo;
    uint32_t columnNo;
};

class Tokenizer {
    std::vector<Token> tokens;
    std::vector<Token>::iterator it;
    void tokenize(std::ifstream&);

    public:
    Tokenizer(const fs::path& jackPath);
    Token getNext();
    bool hasNext();
    void printTokens();
};

#endif
