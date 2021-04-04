#include <algorithm>
#include <string>
#include <utility>
#include "symbol_table.hpp"

void SymbolTable::clear(const Scope& scope) {
    if(scope == Scope::CLASS) {
        staticIndex = 0;
        fieldIndex = 0;
    }
    else {
        argIndex = 0;
        lclIndex = 0;
    }

    auto pred = [&scope](const auto& p) { return p.first.second == scope; };
    for (auto it = table.begin(), last = table.end(); it != last; ) {
        if (pred(*it))
            it = table.erase(it);
        else
            ++it;
    }
}

uint16_t SymbolTable::count(const SymbolKind& kind) {
    auto pred = [&kind](const auto& p) { return p.second.kind == kind; };
    return std::count_if(table.begin(), table.end(), pred);
}

const SymbolTableEntry& SymbolTable::getEntry(const std::string& name) {
    try {
        return table.at(std::make_pair(name, Scope::SUBROUTINE));
    }
    catch (std::out_of_range& err) {
        return table.at(std::make_pair(name, Scope::CLASS));
    }
}

uint16_t SymbolTable::getIndex(const std::string& name) {
    return getEntry(name).index;
}

const SymbolKind& SymbolTable::getKind(const std::string& name) {
    return getEntry(name).kind;
}

const std::string& SymbolTable::getType(const std::string& name) {
    return getEntry(name).type;
}

void SymbolTable::insert(const std::string& name, const std::string& type, const SymbolKind& kind) {
    uint16_t index;
    Scope scope;

    switch(kind) {
        case SymbolKind::STATIC:
            index = staticIndex++;
            scope = Scope::CLASS;
        break;

        case SymbolKind::FIELD:
            index = fieldIndex++;
            scope = Scope::CLASS;
        break;

        case SymbolKind::ARGUMENT:
            index = argIndex++;
            scope = Scope::SUBROUTINE;
        break;

        case SymbolKind::LOCAL:
            index = lclIndex++;
            scope = Scope::SUBROUTINE;
        break;
    }

    table[{name, scope}] = {type, kind, index};
}
