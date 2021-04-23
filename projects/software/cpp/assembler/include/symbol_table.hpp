#ifndef __SYMBOL_TABLE__
#define __SYMBOL_TABLE__

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
        SymbolTable() {}

        SymbolTable(const Map<T>& map) {
            this->table(map);
        }

        void set(const std::string& symbol, const T& address) {
            table[symbol] = address;
        }

        T get(const std::string& symbol) const {
            return table.at(symbol);
        }

        bool has(const std::string& symbol) const {
            auto it = table.find(symbol);
            return it != table.end(); 
        }
};

#endif
