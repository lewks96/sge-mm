#include "src/mm.hpp"
#include <iostream>

int main()
{
    auto allocator = mm::create_stdlib_allocator();
    auto pp = allocator->allocate(100);

    // allocator->free(pp);
    return 0;
}
