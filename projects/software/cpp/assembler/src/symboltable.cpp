#include <string>
#include <unordered_map>
#include "../include/SymbolTable.hpp"

template<class T, class=typename std::enable_if<std::is_unsigned<T>::value, T>::type>
SymbolTable<T>::SymbolTable(const std::unordered_map<std::string, T>& map) {
    int a = 1;
}

//SymbolTable<uint16_t> s;
