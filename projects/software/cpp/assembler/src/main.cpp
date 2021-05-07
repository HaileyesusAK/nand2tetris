#include <stdexcept>
#include "assembler.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	if(argc != 2) {
		throw length_error(string("Usage: ") + argv[0] + string(" <path>"));
		return -1;
	}

	auto assembler = Assembler(argv[1]);
	assembler.generate();

	return 0;
}
