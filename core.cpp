#include "include/corecomponent.hpp"

#include <iostream>
#include <type_traits>
#include <concepts>
#include <unordered_map>


int main() {
    uint32_t exchange_counter = 0;

    CoreComponent cc;
    cc.Run();

    return 0;
}