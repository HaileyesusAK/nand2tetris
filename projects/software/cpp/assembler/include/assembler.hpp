#ifndef __ASSEMBLER_HPP__
#define __ASSEMBLER_HPP__

#include <filesystem>
#include <string>

#include "symbol_table.hpp"

namespace fs = std::filesystem;

class Assembler {
	private:
		SymbolTable<uint16_t> symbolTable; 
        fs::path inputPath;
		static std::string compact(const std::string&);

    public:
		Assembler(const fs::path& path);
        void generate();
};

#endif
