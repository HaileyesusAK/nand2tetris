#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

#include <algorithm>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

template<class T>
using addrT = typename std::enable_if<std::is_unsigned<T>::value, T>::type;

template<class T, class=addrT<T>>
using Map = std::unordered_map<std::string, T>;

template<class T, class=addrT<T>>
class SymbolTable {
    private:
        Map<T> table;

    public:
        T get(const std::string& symbol) const {
            if(symbol.empty())
                throw std::domain_error("Empty string");

            try {
                return table.at(symbol);
            }
            catch (std::out_of_range& ex) {
                throw std::out_of_range("Symobl '" + symbol + "' doesn't exist");
            }
        }

        bool set(const std::string& symbol, T addr) {
            return table.emplace(symbol, addr).second;
        }

        bool has(const std::string& symbol) {
            return table.count(symbol) == 1;
        }
};

#endif
