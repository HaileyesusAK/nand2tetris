#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <stdexcept>
#include "tokenizer.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <stdexcept>

using namespace std;
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input file> <output_file>\n";
        return -1;
    }

    Tokenizer tokenizer {argv[1]};
    std::ofstream output_file {argv[2]};
    if(!output_file.is_open())
        throw std::runtime_error(std::strerror(errno) + std::string(": ") + argv[2]);
    tokenizer.writeXml(output_file);

    /*
    while(tokenizer.hasNext()) {
        Token token = tokenizer.getNext();
        std::cout << "(" << token.lineNo << "," << token.columnNo << ")\t" << token.value << std::endl;
    }
    */
    return 0;
}
