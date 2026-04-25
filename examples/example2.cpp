#include "print.hpp"
#include <fstream>

int main() {
    std::ofstream file("log.txt");
    print("hello", file);
    file.close();
    return 0;
}
