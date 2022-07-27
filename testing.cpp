#include "src/mm.hpp"
#include <iostream>

/*
 *  mm-sge - Simple Game Engine Memory Manager
 */

void test_stdlib_allocator()
{
    auto allocator = mm::create_stdlib_allocator();
    constexpr size_t IterCount = 1024;

    for (auto i = 0; i < IterCount; i++) {
        auto p = allocator->allocate(i);
        if (p) {
            allocator->free(p);
        } else {
            std::cerr << "allocation '" << i << "' failed\n";
            return;
        }
    }
}

int main()
{
    test_stdlib_allocator();
    return 0;
}
