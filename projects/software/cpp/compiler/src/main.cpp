#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <stdexcept>

#include "analyzer.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input path>" << endl;
        return -1;
    }

	fs::path path {argv[1]};
	if(!fs::exists(path))
		throw std::runtime_error(path.string() + " doesn't exist.");
	
	auto is_dir = fs::is_directory(path);
	auto is_reg = fs::is_regular_file(path);

	if((is_reg && path.extension() != ".jack") || (!is_reg && !is_dir)) 
		throw std::runtime_error(path.string() + " must be a Jack file or a directory.");

	std::vector<fs::path> pathes;
	if(fs::is_regular_file(path)) {
		pathes.push_back(path);
	}
	else {
		for(auto& p: fs::directory_iterator(path)) {
			if(p.path().extension() == ".jack")
				pathes.push_back(p.path());
		}
	}
	
	for(auto& path: pathes) {
		Analyzer analyzer {path, 4};
		analyzer.genClass();
		analyzer.writeXml();
	}	

    return 0;
}
