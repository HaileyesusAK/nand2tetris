#include <iostream>
#include <string>
#include "tokenizer.hpp"

using namespace std;
int main(int argc, char* argv[]) {
    Tokenizer tokenizer {argv[1]};

    while(tokenizer.hasNext()) {
        Token token = tokenizer.getNext();
        std::cout << "(" << token.lineNo << "," << token.columnNo << ")\t" << token.value << std::endl;
    }
    return 0;
}
