//
// Created by Lewis on 27/07/2022.
//

#ifndef MM_ALLOCATOR_STDLIB_H
#define MM_ALLOCATOR_STDLIB_H
#include "allocator_interface.h"
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <vector>

namespace mm {

class allocator_stdlib : public allocator_interface {
private:
#ifndef NDEBUG
    // for 'profiling'
    static constexpr size_t PreAllocatedDescriptors = 64;
    struct alloc_desc {
        void* ptr {};
        size_t size {};
        bool hasBeenFreed {};
    };
    std::mutex m_descriptorsLock {};
    std::vector<alloc_desc> m_descriptors {};
#endif

    inline void mark_alloc(void* ptr, size_t size)
    {
#ifndef NDEBUG
        alloc_desc allocDesc {};
        allocDesc.ptr = ptr;
        allocDesc.size = size;
        allocDesc.hasBeenFreed = false;

        std::scoped_lock<std::mutex> lock(m_descriptorsLock);
        m_descriptors.emplace_back(allocDesc);
#endif
    }

    inline void mark_free(void* ptr)
    {
#ifndef NDEBUG
        if (ptr) {
            std::scoped_lock<std::mutex> lock(m_descriptorsLock);
            for (auto& elem : m_descriptors) {
                if (elem.ptr == ptr) {
                    elem.hasBeenFreed = true;
                    return;
                }
            }
        }
#endif
    }

public:
    allocator_stdlib()
    {
#ifndef NDEBUG
        m_descriptors.reserve(PreAllocatedDescriptors);
#endif
    }

    ~allocator_stdlib() override
    {
#ifndef NDEBUG
        size_t allocations = 0;
        for (auto& elem : m_descriptors) {
            if (!elem.hasBeenFreed) {
                allocations++;
                // maybe free?
                // this->free(elem.ptr);
            }
        }
        if (allocations > 0) {
            std::cerr << "allocator_stdlib: At dtor there where '" << allocations << "' allocations un-freed!\n";
        }
#endif
    }

    [[nodiscard]] void* allocate(size_t size) override
    {
        auto* p = std::malloc(size);
        if (p) {
            mark_alloc(p, size);
            return p;
        }
        return nullptr;
    }

    [[nodiscard]] void* reallocate(void* ptr, size_t size) override
    {
        auto* p = std::realloc(ptr, size);
        if (p) {
            if (p != ptr) {
                mark_free(ptr);
                mark_alloc(p, size);
            }
            return p;
        }
        return nullptr;
    }

    void free(void* ptr) override
    {
        if (ptr) {
            mark_free(ptr);
            std::free(ptr);
        }
    }

    [[nodiscard]] size_t get_used() const override
    {
        size_t size = 0;
        for (const auto& elem : m_descriptors) {
            if (!elem.hasBeenFreed) {
                size = elem.size;
            }
        }
        return size;
    }

    [[nodiscard]] size_t get_capacity() const override
    {
        return 0;
    }
};

}

#endif // MM_ALLOCATOR_STDLIB_H
