#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include "../mpark/variant.hpp"
#include "SymbolTable.h"
struct Struct {
    std::string name;
    SymbolTable table;
    
};

class StructTable {

public:
    void insert(const Struct symbol) {
        table[symbol.name] = symbol;
    }

    Struct* lookup(const std::string& name) {
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
            std::cout << "Name: " << pair.second.name << "\n";
        }
    }

private:
    std::unordered_map<std::string, Struct> table;
};