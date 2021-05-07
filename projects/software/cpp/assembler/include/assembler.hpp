#ifndef __ASSEMBLER_HPP__
#define __ASSEMBLER_HPP__

#include <filesystem>
#include <string>
#include "symbol_table.hpp"

namespace fs = std::filesystem;

class Assembler {
    private:
        fs::path inputPath;
        static std::string compact(const std::string&);
        void generate(const fs::path& path);
        SymbolTable<uint16_t> generateSymbolTable(const fs::path& path);

    public:
        Assembler(const fs::path& path);
        void generate();
};

#endif
