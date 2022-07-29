//
// Created by Lewis on 28/07/2022.
//

#ifndef MM_ALLOCATOR_LINEAR_H
#define MM_ALLOCATOR_LINEAR_H
#include "allocator_interface.h"
#include <cstdint>
#include <memory>

namespace mm {

/*
 * linear allocator - no reallocation or freeing
 */
class allocator_linear : public allocator_interface {
private:
    size_t m_linearCapacity {};
    size_t m_linearUsed {};
    std::unique_ptr<uint8_t[]> m_allocation {};

public:
    explicit allocator_linear(size_t slabSize)
    {
        m_allocation = std::make_unique<uint8_t[]>(slabSize);
        m_linearCapacity = slabSize;
    }

    ~allocator_linear() override
    {
        m_allocation.reset();
    }

    [[nodiscard]] void* allocate(size_t size) override
    {
        if (size + m_linearUsed < m_linearCapacity) {
            auto* pAlloc = m_allocation.get() + m_linearUsed;
            m_linearUsed += size;
            return pAlloc;
        }
        return nullptr;
    }

    [[nodiscard]] void* reallocate(void* ptr, size_t size) override
    {
        return nullptr;
    }

    void free(void* ptr) override
    {
    }

    [[nodiscard]] size_t get_used() const override
    {
        return m_linearUsed;
    }

    [[nodiscard]] size_t get_capacity() const override
    {
        return m_linearCapacity;
    }
};

}

#endif // MM_ALLOCATOR_LINEAR_H
