#ifndef __UTILS_H__
#define __UTILS_H__

#include <filesystem>
#include <string>
#include <utility>
#include "generator.hpp"

namespace fs = std::filesystem;

std::pair<std::string, int> execute(const std::string& cmd);
void saveAsm(const AsmInst& insts, const fs::path& path);

#endif
