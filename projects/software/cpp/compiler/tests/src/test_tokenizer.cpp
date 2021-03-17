#include <filesystem>

#include "catch.hpp"
#include "tokenizer.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

TEST_CASE("Tokenizer", "[generateXml]") {
	fs::path dir = DATA_DIR / "Square/Square";

	fs::path pathPrefix = DATA_DIR / dir;
	fs::path input = pathPrefix;
	input += ".jack";
	fs::path output = pathPrefix;
	output += "T.xml";

	Tokenizer tokenizer = Tokenizer(input);
	tokenizer.generateXml();

	fs::path expectedPath = EXP_DATA_DIR / dir;
	expectedPath += "T.xml";

	REQUIRE(cmpFiles(expectedPath, output));
}
