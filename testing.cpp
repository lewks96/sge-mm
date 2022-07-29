#include "src/mm.hpp"
#include <iostream>

/*
 *  mm-sge - Simple Game Engine Memory Manager
 */

void test_allocator(std::unique_ptr<mm::allocator_interface> allocator)
{
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
    constexpr size_t DefaultAllocatorSize = 1024 * 1024 * 32;

    auto allocator = mm::create_pool_allocator(DefaultAllocatorSize);
    test_allocator(std::move(allocator));

    return 0;
}
