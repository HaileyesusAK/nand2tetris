#ifndef __UTILS_H__
#define __UTILS_H__

#include <filesystem>

namespace fs = std::filesystem;

bool cmpFiles(const fs::path& p1, const fs::path& p2);

#endif
