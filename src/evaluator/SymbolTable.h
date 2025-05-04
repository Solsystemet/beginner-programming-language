#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include "../mpark/variant.hpp"
#include "StructTable.h"

struct Struct;

struct Symbol {
	std::string name;
	std::string type;
    bool isAnArray = false;
    mpark::variant<double,
        std::string,
        bool,
        Struct*,
        std::vector<mpark::variant<double, std::string, bool, Struct*>>
        > value; // the primitive value a symbol point to
	int scopeLevel; // not using yet
};

class SymbolTable {

public:
    void insert(const Symbol symbol) {
        table[symbol.name] = symbol;
    }

    Symbol* lookup(const std::string& name) {
        auto it = table.find(name);
        if (it != table.end()) {
            return &it->second;
        }
        return nullptr;
    }
    bool contains(const std::string& name)
    {
        if (table.empty())
            return false;
        auto it = table.find(name);
        if (it != table.end()) {
            return true;
        }
        return false;
    }
    // dev tool
    void print() const {
        for (const auto& pair : table) {
            std::cout << "Name: " << pair.second.name
                << ", Type: " << pair.second.type
                << ", Scope: " << pair.second.scopeLevel << "\n";
        }
    }

private:
    std::unordered_map<std::string, Symbol> table;
};