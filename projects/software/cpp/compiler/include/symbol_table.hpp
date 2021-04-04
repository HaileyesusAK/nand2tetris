#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <cstdint>
#include <string>
#include <unordered_map>

enum class Scope {
    CLASS,
    SUBROUTINE
};

enum class SymbolKind {
    FIELD,
    STATIC,
    LOCAL,
    ARGUMENT
};

struct SymbolTableEntry {
	std::string type;
	SymbolKind kind;
	uint16_t index;
};

using Key = std::pair<std::string, Scope>;
//std::unordered_map uses std::hash for its key but std::hash doesn't work for std::pair
struct KeyHash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const
    {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

class SymbolTable {
    //std::unordered_map<std::pair<std::string, Scope>, SymbolTableEntry> table;
    std::unordered_map<Key, SymbolTableEntry, KeyHash> table;
    uint16_t classIndex = 0;
    uint16_t subroutineIndex = 0;

    public:
    void clear(const Scope& scope);
    uint16_t count(const SymbolKind& kind);
    uint16_t getIndex(const std::string& name);
    const SymbolTableEntry& getEntry(const std::string& name);
    const SymbolKind& getKind(const std::string& name);
    const std::string& getType(const std::string& name);
    void insert(const std::string& type, const SymbolKind& kind, uint16_t& index);
    void insert(const std::string& name, const std::string& type, const SymbolKind& kind);
};

#endif
