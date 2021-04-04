#include <cstdint>
#include <stdexcept>

#include "symbol_table.hpp"
#include "catch.hpp"

TEST_CASE("Same variable names but different scope", "[symbolTable]") {
    SymbolTable table;
    table.insert("x", "int", SymbolKind::FIELD);
    table.insert("x", "char", SymbolKind::ARGUMENT);

    REQUIRE(table.getIndex("x") == 0);
    REQUIRE(table.getKind("x") == SymbolKind::ARGUMENT); // Subroutine scope
    REQUIRE(table.getType("x") == "char"); // Subroutine scope
}

TEST_CASE("Count of symbols", "[symbolTable]") {
    SymbolTable table;
    table.insert("x", "int", SymbolKind::STATIC);
    table.insert("y", "int", SymbolKind::STATIC);
    table.insert("z", "int", SymbolKind::STATIC);
    table.insert("x", "int", SymbolKind::ARGUMENT);
    table.insert("y", "int", SymbolKind::LOCAL);
    table.insert("z", "int", SymbolKind::LOCAL);

    REQUIRE(table.count(SymbolKind::FIELD) == 0);
    REQUIRE(table.count(SymbolKind::STATIC) == 3);
    REQUIRE(table.count(SymbolKind::ARGUMENT) == 1);
    REQUIRE(table.count(SymbolKind::LOCAL) == 2);
}

TEST_CASE("Clearing table", "[symbolTable]") {
    SymbolTable table;

    table.insert("x", "int", SymbolKind::STATIC);
    table.insert("y", "int", SymbolKind::STATIC);
    table.insert("z", "int", SymbolKind::FIELD);
    REQUIRE(table.count(SymbolKind::STATIC) == 2);
    REQUIRE(table.count(SymbolKind::FIELD) == 1);

    //After clearing class-level symbols, there shouldn't exist any symbol at that level
    table.clear(Scope::CLASS);
    REQUIRE(table.count(SymbolKind::STATIC) == 0);
    REQUIRE(table.count(SymbolKind::FIELD) == 0);

    table.insert("x", "int", SymbolKind::STATIC);
    table.insert("y", "int", SymbolKind::FIELD);
    table.insert("z", "int", SymbolKind::FIELD);
    REQUIRE(table.getIndex("z") == 1);

    table.insert("z", "int", SymbolKind::ARGUMENT);
    table.insert("y", "int", SymbolKind::LOCAL);
    table.insert("x", "int", SymbolKind::LOCAL);
    REQUIRE(table.count(SymbolKind::ARGUMENT) == 1);
    REQUIRE(table.count(SymbolKind::LOCAL) == 2);
    REQUIRE(table.getIndex("z") == 0);  // varaible index at a subroutine scope

    //After clearing subroutine-level symbols, there shouldn't exist any symbol at that level
    table.clear(Scope::SUBROUTINE);
    REQUIRE(table.count(SymbolKind::ARGUMENT) == 0);
    REQUIRE(table.count(SymbolKind::LOCAL) == 0);
    REQUIRE(table.getIndex("z") == 1);  // varaible index at a class scope

    //Getting info about a non-existent symbol must throw std::out_of_range
    REQUIRE_THROWS_AS(table.getIndex("bogus"), std::out_of_range);
}
