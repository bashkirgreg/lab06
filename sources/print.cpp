#include "print.hpp"
#include <iostream>

void print(const std::string& text, std::ostream& out) {
    out << text;
}

void print(const std::string& text, std::ofstream& out) {
    out << text;
}
