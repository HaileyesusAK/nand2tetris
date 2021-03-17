#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "analyzer.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

const fs::path DATA_DIR = fs::current_path().parent_path() / "data";
const fs::path EXP_DATA_DIR = DATA_DIR / "expected";

bool cmpFiles(const fs::path& p1, const fs::path& p2) {
	std::ifstream file1(p1), file2(p2);

	std::string line1, line2;
	while(!file1.eof() && !file2.eof()) {
		std::getline(file1, line1);
		std::getline(file2, line2);
		if(line1 != line2)
			return false;
	}

	return (file1.eof() && file2.eof());
}
