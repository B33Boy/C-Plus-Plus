/**
 * @file
 * @brief Linear Allocator
 * @details Implementation of a linear allocator that pre-allocates memory
 * for high speed access. See [Region-based memory
 * management](https://en.wikipedia.org/wiki/Region-based_memory_management).
 *
 * A linear (also known a bump) allocator works by allocating a buffer of
 * memory upon initialization, and then handing it out per allocation call. An
 * index is used to indicate from where to hand out memory, after doing so it
 * will be "bumped up" to the next spot. For this implementation, the next spot
 * is ensured to be aligned to the largest type on this plaform.
 *
 * Note: CPUs access data most efficiently when it is aligned (i.e. data of
 * size N is located at a memory address that is a multiple of N). Alignment of
 * data matters because depending on the platform, when accessing un-aligned
 * data a CPU will have reduced performance or straight up crash. See
 * https://en.wikipedia.org/wiki/Data_structure_alignment for more info.
 *
 * One of the biggest reasons for using a compile-time linear allocator is the
 * fact that lifetime of data is limited to the scope of use. We don't have to
 * worry about freeing up memory as the stack will take care of that for us, and
 * for that reason there is no need for a de-allocate function.
 *
 *
 * Key constraints and limitations of this example:
 * - As mentioned above, this implementation has the buffer aligned to the
 * std::max_align_t which ensures that fundamental types are aligned to the
 * largest type on the current platform. However, when trying to allocate for
 * complex types which may be larger, this doesn't work. One solution would be
 * to take a note from
 * [std::pmr::memory_resource::allocate](https://en.cppreference.com/cpp/memory/memory_resource/allocate)
 * and have the alignment as a default parameter.
 *
 * - It is up to the user to ensure that they do not write past the returned
 * bytes. This is the same behaviour as "malloc" or "new".
 *
 *
 * @author [Abhi Patel](https://github.com/B33Boy/)
 */

#pragma once

#include <cstddef>  /// for std::byte

/**
 * @brief Memory algorithms
 * @namespace memory
 */
namespace memory {
/**
 * @brief allocator algorithm
 * @namespace allocator
 */
namespace allocator {

/**
 * @class linear_allocator
 * @brief Stack based allocator that is used to hand out bytes of memory
 *
 * @tparam N
 */
template <size_t N>
class linear_allocator {
 public:
    constexpr linear_allocator() = default;

    // ================= Special Member Functions =================
    /**
     * For simplicity, we will disallow copying or moving the allocator.
     */

    ~linear_allocator() = default;

    linear_allocator(linear_allocator const&) =
        delete;  ///< disable copy constructor

    linear_allocator(linear_allocator&&) noexcept =
        delete;  ///< disable move constructor

    linear_allocator& operator=(linear_allocator const&) =
        delete;  ///< disable copy assignment operator

    linear_allocator& operator=(linear_allocator&&) noexcept =
        delete;  ///< disable move assignment operator

    // ================= Allocataor API =================
    /**
     * @brief Allocates num_bytes from the internal buffer
     * @details Returns a pointer to the next aligned position in the
     * buffer.
     *
     * @param num_bytes number of bytes to allocate
     * @return std::byte* to allocate memory, or nullptr upon failure
     */
    [[nodiscard]] auto constexpr allocate(size_t num_bytes) noexcept
        -> std::byte* {
        size_t aligned_next = linear_allocator::align(next_ + num_bytes);

        if (aligned_next > N)
            return nullptr;

        auto* mem = &buffer_[next_];
        next_ = aligned_next;

        return mem;
    }

    /**
     * @brief resets allocator to the start
     *
     */
    void reset() noexcept { next_ = 0; }

    /**
     * @brief returns size of data allocated
     *
     * @return size_t next position
     */
    auto size() const noexcept -> size_t { return next_; }

    /**
     * @brief return capacity of the allocator
     *
     * @return size_t capacity
     */
    auto cap() const noexcept -> size_t { return N; }

 private:
    static constexpr size_t alignment = alignof(std::max_align_t);

    alignas(alignment) std::byte buffer_[N];
    size_t next_{0};

    static constexpr auto align(size_t offset) noexcept -> size_t {
        return (offset + (alignment - 1)) & ~(alignment - 1);
    }
};

}  // namespace allocator
}  // namespace memory
