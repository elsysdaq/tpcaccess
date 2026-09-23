#ifndef COMMON_SYSTEM_H
#define COMMON_SYSTEM_H

#ifdef __cplusplus
#include <mutex>

#include <cstddef>
#include <cstdint>
#else
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#endif

/**
 * Platform-specific memory management functionality
 * Provides interface to linux and win32 page-based memory allocation methods, including support for large page
 * allocations. Large pages are useful e.g. with DMA buffers where physical contiguity reduces the length of sg-lists.
 */

#ifdef __cplusplus
namespace common {
extern "C" {
#endif  // __cplusplus

// Get size of system memory pages
size_t sysPageSize(void);
// Get size of large memory pages (called "huge" in linux). Returns 0 if system does not support large pages.
size_t sysLargePageSize(void);

// True if the process' call to initLargePaging() has suceeded. Does not guarantee that allocation will succeed.
bool largePagingIsEnabled(void);

// Initialization routine to allow system requests for large memory pages. Returns true if successful.
bool initLargePaging(void);

#ifdef __cplusplus
// Platform-specific memory allocation function using VirtualAlloc/Win32 and mmap/Linux
// Allocation size is rounded up to a multiple of system's page size if allocated_size is non-null
void* allocPages(size_t size, bool use_large_pages = false, size_t* allocated_size = nullptr);
#else
void* allocPages(size_t size, bool use_large_pages, size_t* allocated_size);
#endif

// Free a buffer that was allocated with allocPages
void freePages(void* buf, size_t size);

#ifdef __cplusplus
}  // extern "C"

/**
 * A memory arena backed by large pages. Uses dlmalloc to manage the buffer contents.
 *
 * Usage Example:
 * --------------
 * LargePageArena arena(2 * 1024 * 1024);         // 2MB arena
 *
 * void* ptr = arena.allocate(1024);              // 1KB, default alignment
 * void* aligned = arena.allocate(4096, 64);      // 4KB, 64-byte aligned
 * arena.deallocate(ptr);
 * arena.deallocate(aligned);
 */

class LargePageArena {
   public:
    explicit LargePageArena(size_t size = common::sysLargePageSize());
    ~LargePageArena();

    LargePageArena(const LargePageArena&)            = delete;
    LargePageArena& operator=(const LargePageArena&) = delete;
    LargePageArena(LargePageArena&&)                 = delete;
    LargePageArena& operator=(LargePageArena&&)      = delete;

    [[nodiscard]] void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
    void deallocate(void* ptr) noexcept;

   private:
    std::mutex m_mutex{};
    void* m_mem{};
    size_t m_size{};

    using mspace = void*;
    mspace m_mspace{};
};

}  // namespace common
#endif  // __cplusplus

#endif  // COMMON_SYSTEM_H
