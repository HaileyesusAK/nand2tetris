#include <iostream>
#include <stdexcept>

#include "symbol_table.hpp"
#include "utils.hpp"

#include <CppUTest/TestHarness.h>

using namespace std;

TEST_GROUP(SymbolTable)
{

};

TEST(SymbolTable, set_get)
{
    SymbolTable<uint16_t> symTable;
    uint16_t addr = 16;
    CHECK_EQUAL(true, symTable.set("radius", addr));
    CHECK_EQUAL(false, symTable.set("radius", addr));   // Must not overwrite existing symbol address
    CHECK_EQUAL(addr, symTable.get("radius"));
}

TEST(SymbolTable, has)
{
    SymbolTable<uint16_t> symTable;
    symTable.set("area", 320);
    CHECK_EQUAL(true, symTable.has("area"));
    CHECK_EQUAL(false, symTable.has("radius"));
}

TEST(SymbolTable, get_empty_symbol)
{
    SymbolTable<uint16_t> symTable;
    CHECK_THROWS(domain_error, symTable.get(""));
}

TEST(SymbolTable, non_existing_symbol)
{
    SymbolTable<uint16_t> symTable;
    CHECK_THROWS(out_of_range, symTable.get("radius"));
}
