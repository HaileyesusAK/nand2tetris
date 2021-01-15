#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <utility>
#include "generator.hpp"

std::pair<std::string, int> execute(const std::string& cmd);
void saveAsm(const AsmInst& insts, const std::string& filename);
std::pair<std::string, int> test(const std::string& filename, const AsmInst& insts);

#endif
