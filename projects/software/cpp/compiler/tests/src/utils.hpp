#ifndef __UTILS_H__
#define __UTILS_H__

#include <filesystem>
#include <utility>
#include "analyzer.hpp"

namespace fs = std::filesystem;
static const fs::path DATA_DIR = fs::current_path().parent_path() / "data";

bool cmpFiles(const fs::path& p1, const fs::path& p2);

#endif
