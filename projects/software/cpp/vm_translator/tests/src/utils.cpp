#include <array>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <utility>
#include "utils.hpp"

std::pair<std::string, int> execute(const std::string& c) {
    std::array<char, 128> buffer;
    std::string result;
    std::string cmd(c + " 2>&1");
    auto pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }
    
    return std::make_pair(result, pclose(pipe));
}
