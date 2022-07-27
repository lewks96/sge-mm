//
// Created by Lewis on 27/07/2022.
//

#ifndef MM_MM_HPP
#define MM_MM_HPP
#include "allocator_interface.h"
#include "allocator_stdlib.h"
#include <memory>

namespace mm {

static inline std::unique_ptr<allocator_interface> create_stdlib_allocator()
{
    return std::make_unique<allocator_stdlib>();
}

}

#endif // MM_MM_HPP
