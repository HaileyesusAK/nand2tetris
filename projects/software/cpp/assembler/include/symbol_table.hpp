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
        const T BASE_ADDR = 16;
        T currAddr = BASE_ADDR;

    public:
        SymbolTable() {}
        SymbolTable(T base_addr) : BASE_ADDR(base_addr) {}

        T get(const std::string& symbol) {
            if(symbol.empty())
                throw std::domain_error("Empty string");

            if(std::all_of(symbol.begin(), symbol.end(), ::isdigit)) {
                return static_cast<T>(std::stoul(symbol));
            }
            else {
                try {
                    return table.at(symbol);
                }
                catch (std::out_of_range& ex) {
                    table.emplace(symbol, currAddr);
                    return currAddr++;
                }
            }
        }

        void set(const std::string& symbol, T addr) {
            table.insert_or_assign(symbol, addr);
        }

        bool has(const std::string& symbol) { return table.count(symbol) == 1; }
};

#endif
