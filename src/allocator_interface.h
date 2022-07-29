//
// Created by Lewis on 27/07/2022.
//

#ifndef MM_ALLOCATOR_INTERFACE_H
#define MM_ALLOCATOR_INTERFACE_H
#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>
#include <string>

namespace mm {

// cool little helpers for working out sizes
namespace size_helpers {
    constexpr size_t as_kb(size_t val)
    {
        const auto size = 1024 * val;
        assert(size < std::numeric_limits<size_t>::max());
        return size;
    }
    constexpr size_t as_mb(size_t val)
    {
        const auto size = static_cast<size_t>(1024 * 1024) * val;
        assert(std::numeric_limits<size_t>::max() > size);
        return size;
    }
    constexpr size_t as_gb(size_t val)
    {
        const auto size = static_cast<size_t>(1024 * 1024 * 1024) * val;
        assert(size < std::numeric_limits<size_t>::max());
        return size;
    }

    constexpr size_t operator"" _kb(size_t val)
    {
        return as_kb(val);
    }
    constexpr size_t operator"" _mb(size_t val)
    {
        return as_mb(val);
    }
    constexpr size_t operator"" _gb(size_t val)
    {
        return as_gb(val);
    }
}

// common utility
namespace util {
    template <typename T>
    constexpr bool is_power_of_two(T x)
    {
        static_assert(std::is_integral<T>::value, "invalid type, use integrals");
        return (x != 0) && ((x & (x - 1)) == 0);
    }

    // msvc doesn't support std::aligned_alloc/free
    inline void* aligned_alloc(size_t size, size_t alignment)
    {
#ifdef _MSC_VER
        return _aligned_malloc(size, alignment);
#else
        return std::aligned_alloc(size, alignment);
#endif
    }

    inline void aligned_free(void* ptr)
    {
#ifdef _MSC_VER
        return _aligned_free(ptr);
#else
        std::aligned_free(size, alignment); //?
#endif
    }

    inline void fatal_if(bool condition, const std::string& errorMessage = "")
    {
        if (condition) {
#ifdef ALLOW_EXCEPTIONS
            throw std::runtime_error(errorMessage);
#endif
            // idea: maybe report the errorMessage somehow?
            std::abort();
        }
    }
}

class allocator_interface {
public:
    // Can't copy allocators!
    allocator_interface() = default;
    allocator_interface(const allocator_interface&) = delete;
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
