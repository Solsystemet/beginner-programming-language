#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include "../mpark/variant.hpp"
#include "../nodes.hpp"
#include "SymbolTable.h"
struct Function {
    std::string name;
    // return types
    std::string type = "";
    bool isAnArray = false;

    // function stores arguments in its own symbol table
    SymbolTable args;

    // The beginning of the functions code content
    node::NodeFunctionStmt* stmts;
};

class FunctionTable {

public:
    void insert(const Function func) {
        table[func.name] = func;
    }

    Function* lookup(const std::string& name) {
        auto it = table.find(name);
        if (it != table.end()) {
            return &it->second;
        }
        return nullptr;
    }
    bool contains(const std::string& name)
    {
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
                << "\n";
        }
    }

private:
    std::unordered_map<std::string, Function> table;
};