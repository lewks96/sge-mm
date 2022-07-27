//
// Created by Lewis on 27/07/2022.
//

#ifndef MM_ALLOCATOR_INTERFACE_H
#define MM_ALLOCATOR_INTERFACE_H
#include <cstddef>

namespace mm {

class allocator_interface {
public:
    virtual ~allocator_interface() = default;

    // generic allocation function
    [[nodiscard]] virtual void* allocate(size_t size) = 0;

    // preform a reallocation
    [[nodiscard]] virtual void* reallocate(void* ptr, size_t size) = 0;

    // free allocation
    virtual void free(void* ptr) = 0;

    // get the total memory used by the allocator
    [[nodiscard]] virtual size_t get_used() const = 0;

    // get the total capacity that the allocator has
    [[nodiscard]] virtual size_t get_capacity() const = 0;
};

}

#endif // MM_ALLOCATOR_INTERFACE_H
