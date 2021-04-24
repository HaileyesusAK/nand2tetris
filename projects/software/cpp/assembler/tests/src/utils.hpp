#ifndef __UTILS_H__
#define __UTILS_H__

#include <filesystem>

namespace fs = std::filesystem;

extern const fs::path DATA_DIR;
extern const fs::path EXP_DATA_DIR;

bool cmpFiles(const fs::path& p1, const fs::path& p2);

#endif
