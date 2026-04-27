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
 * to take a note from *
 * [std::pmr::memory_resource::allocate](https://en.cppreference.com/cpp/memory/memory_resource/allocate)
 * and have the alignment as a default parameter.
 *
 * - It is up to the user to ensure that they do not write past the returned
 * bytes. This is the same behaviour as "malloc" or "new".
 *
 *
 * @author [Abhi Patel](https://github.com/B33Boy/)
 */

#include "linear_allocator.hpp"

#include <cassert>   /// for std::assert
#include <iostream>  /// for IO operations

/**
 * @brief Memory algorithms
 * @namespace memory
 */
namespace memory {

/**
 * @brief allocator algorithms
 * @namespace allocator
 */
namespace allocator {

/**
 * @brief Self-test implementations
 * @returns void
 */

static void test_allocator_allocates_data() {
    static const size_t CAPACITY = 16;
    linear_allocator<CAPACITY> la{};

    auto* mem = la.allocate(sizeof(int));
    int* num_ptr = new (mem) int{69};  ///< use placement new to initialize an
                                       ///< int at the memory we allocated

    assert(69 == *num_ptr);
    assert(4 == la.size());
    assert(16 == la.cap());
}

static void test_allocator_resets() {
    static const size_t CAPACITY = 8;
    linear_allocator<CAPACITY> la{};

    auto* mem = la.allocate(sizeof(int));
    int* num_ptr = new (mem) int{69};

    la.reset();
    assert(0 == la.size());

    // Allocate memory at the same location as the first, should hand out the
    // same bytes
    auto* new_mem = la.allocate(sizeof(int));
    int* new_num_ptr = new (new_mem) int{10};

    assert(10 == *new_num_ptr);
    assert(*new_num_ptr == *num_ptr);
}

static void test_allocate_when_full_returns_nullptr() {
    static const size_t CAPACITY = sizeof(double);
    linear_allocator<CAPACITY> la{};

    auto* mem = la.allocate(sizeof(double));
    double* _ = new (mem) double{69.0};

    auto* some_char = la.allocate(sizeof(char));
    assert(nullptr == some_char);
}

static void tests() {
    test_allocator_allocates_data();
    test_allocator_resets();
    test_allocate_when_full_returns_nullptr();

    std::cout << "All tests have successfully passed!\n";
}

/**
 * @brief Main function
 * @returns 0 on exit
 */
int main() {
    tests();

    return 0;
}

}  // namespace allocator
}  // namespace memory