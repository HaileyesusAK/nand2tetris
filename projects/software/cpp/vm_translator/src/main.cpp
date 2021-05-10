#include <stdexcept>
#include <filesystem>
#include "translator.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 2) {
		throw length_error(string("Usage: ") + argv[0] + string(" <path>"));
		return -1;
	}

	Translator translator;
	translator.translate(std::filesystem::path(argv[1]));

	return 0;
}
