#include "main.h"
#include <string>
#include "nlohmann/json.hpp"

struct object {
    std::string name;
    int age;
};

int main() {
    Main main;
    main.run();
    return 0;
}

