//
// Created by Lewis on 27/07/2022.
//

#ifndef MM_MM_HPP
#define MM_MM_HPP
#include "allocator_interface.h"
#include "allocator_linear.h"
#include "allocator_pool.h"
#include "allocator_stdlib.h"
#include <memory>

namespace mm {

static inline std::unique_ptr<allocator_interface> create_stdlib_allocator()
{
    return std::make_unique<allocator_stdlib>();
}

static inline std::unique_ptr<allocator_interface> create_linear_allocator(size_t size)
{
    return std::make_unique<allocator_linear>(size);
}

static inline std::unique_ptr<allocator_interface> create_pool_allocator(size_t size, size_t segmentSize = 0, size_t alignment = 0)
{
    if (segmentSize == 0) {
        segmentSize = allocator_pool::default_segment_size;
    }
    if (alignment == 0) {
        alignment = allocator_pool::default_alignment;
    }
    return std::make_unique<allocator_pool>(size, segmentSize, alignment);
}

}

#endif // MM_MM_HPP
