//
// Created by Lewis on 28/07/2022.
//

#ifndef MM_ALLOCATOR_POOL_H
#define MM_ALLOCATOR_POOL_H

#include "allocator_interface.h"
#include <cstdint>
#include <memory>

namespace mm {

/*
 * pool allocator - generic pool allocator
 */
class allocator_pool : public allocator_interface {
private:
    enum class allocation_status_flags : uint32_t {
        null = 0x00000000,
        free = 0x00000001,
        used = 0x00000002,
        dirty = 0x00000004,
        linked = 0x00000008,
    };
    constexpr friend allocation_status_flags operator|(allocation_status_flags a, allocation_status_flags b)
    {
        return static_cast<allocation_status_flags>(static_cast<int>(a) | static_cast<int>(b));
    }
    constexpr friend bool operator&(allocation_status_flags a, allocation_status_flags b)
    {
        return static_cast<bool>(static_cast<int>(a) & static_cast<int>(b));
    }
    static constexpr uint32_t SegmentMagic = 0xffbbad0;

    struct segment_desc {
        uint32_t magic {};
        void* ptr { nullptr };
        size_t size { 0 };
        size_t id { 0 };
        allocation_status_flags status { allocation_status_flags::null };
        segment_desc* linkedChainStart { nullptr };
        segment_desc* linkedChainEnd { nullptr };
        uint32_t linkedChainBlockCount { 0 };
        uint32_t linkedChainBlockIndex { 0 };
    };
    static constexpr bool is_segment_valid(segment_desc* desc)
    {
        if (desc) {
            if (desc->magic == SegmentMagic) {
                return true;
            }
        }
        return false;
    }

    size_t m_poolCapacity {};
    size_t m_poolUsed {};
    size_t m_segmentCount {};
    size_t m_segmentSize {};
    size_t m_middleSegment {};
    std::unique_ptr<uint8_t*> m_allocation {};
    std::unique_ptr<segment_desc[]> m_segmentTable {};

    void reset_table()
    {
        segment_desc* desc {};
        uint8_t* ptr = *m_allocation;
        for (size_t i = 0; i < m_segmentCount; ++i) {
            desc = &m_segmentTable[i];
            desc->ptr = ptr;
            desc->id = i;
            desc->size = m_segmentSize;
            desc->magic = SegmentMagic;
            desc->status = allocation_status_flags::free;
            ptr += m_segmentSize;
        }
    }

    void* allocate_single_from(size_t index)
    {
        segment_desc* segment {};
        for (size_t i = index; i < m_segmentCount; ++i) {
            segment = &m_segmentTable[i];
            if (is_segment_valid(segment)) {
                if (segment->status == allocation_status_flags::free || segment->status == allocation_status_flags::dirty) {
                    segment->status = allocation_status_flags::used;
                    m_poolUsed += m_segmentSize;
                    return segment->ptr;
                }
            }
        }
        return nullptr;
    }

public:
    static constexpr size_t default_segment_size = size_helpers::as_kb(32);
    static constexpr size_t default_alignment = 16;

    explicit allocator_pool(size_t poolSize,
        size_t segmentSize = default_segment_size,
        size_t alignment = default_alignment)
    {
        // if we can't allocate like 2 segments just error out
        util::fatal_if(poolSize < (sizeof(segment_desc) + segmentSize) * 2);
        util::fatal_if(!util::is_power_of_two(poolSize), "poolSize must be to the power of 2");
        util::fatal_if(!util::is_power_of_two(segmentSize), "segmentSize must be to the power of 2");

        m_segmentCount = poolSize / segmentSize;
        m_segmentSize = segmentSize;
        m_middleSegment = m_segmentCount / 2;
        m_poolCapacity = poolSize;
        m_segmentTable = std::make_unique<segment_desc[]>(m_segmentCount);
        m_allocation = std::make_unique<uint8_t*>(static_cast<uint8_t*>(util::aligned_alloc(m_poolCapacity, alignment)));

        std::memset(m_segmentTable.get(), '\0', sizeof(segment_desc) * m_segmentCount);
        std::memset(*m_allocation, '\0', m_poolCapacity);

        reset_table();
    }

    ~allocator_pool() override = default;

    [[nodiscard]] void* allocate(size_t size) override
    {
        if (size < m_segmentSize) {
            return allocate_single_from(0);
        } else {
            // TODO: Support linking multiple blocks + cache last freed
        }
        return nullptr;
    }

    [[nodiscard]] void* reallocate(void* ptr, size_t size) override
    {
        // TODO
        return nullptr;
    }

    void free(void* ptr) override
    {
        if (ptr) {
            segment_desc* segment {};
            for (size_t i = 0; i < m_segmentCount; ++i) {
                segment = &m_segmentTable[i];
                if (is_segment_valid(segment)) {
                    if (segment->ptr == ptr) {
                        if (segment->status & allocation_status_flags::linked) {
                            // TODO: Handle freeing linked chains
                        }
                        segment->status = allocation_status_flags::dirty;
                        m_poolUsed -= m_segmentSize;
                        return;
                    }
                }
            }
        }
    }

    [[nodiscard]] size_t get_used() const override
    {
        return m_poolUsed;
    }

    [[nodiscard]] size_t get_capacity() const override
    {
        return m_poolCapacity;
    }
};

}

#endif // MM_ALLOCATOR_POOL_H
