#include <iostream>
#include <stdexcept>

#include "symbol_table.hpp"
#include "utils.hpp"

#include <CppUTest/TestHarness.h>

using namespace std;

TEST_GROUP(SymbolTable)
{

};

TEST(SymbolTable, default_constructor)
{
    SymbolTable<uint16_t> symTable;
    auto addr = symTable.get("radius");

    //The first symbol must be assigned at the base address (default 16)
    CHECK_EQUAL(addr, 16);
}

TEST(SymbolTable, constructor)
{
    SymbolTable<uint16_t> symTable(30);
    symTable.get("radius");
    auto addr = symTable.get("area");

    //The second symbol must be BASE_ADDR + 1
    CHECK_EQUAL(addr, 31);
}

TEST(SymbolTable, empty_symbol)
{
    SymbolTable<uint16_t> symTable;

    CHECK_THROWS(domain_error, symTable.get(""));
}

TEST(SymbolTable, set)
{
    SymbolTable<uint16_t> symTable;
    symTable.get("radius");
    symTable.set("radius", 300);

    CHECK_EQUAL(300, symTable.get("radius")); // must be 300 instead of the original 16
}

TEST(SymbolTable, number_symbol)
{
    SymbolTable<uint16_t> symTable;
    auto addr = symTable.get("1246");
    
    //A symbol composed of only numbers must be returned as-is, converted to an int
    CHECK_EQUAL(1246, addr);
}
