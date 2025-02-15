#pragma once

#include <string>

class Interpreter {
public:
    Interpreter();
    ~Interpreter();
    std::string Run(const std::string&);
};
