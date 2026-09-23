#include "sys_memory.h"

#include "dlmalloc_mspace.h"
#include "logging.h"
#include "utils/tpcmath.h"
#include "utils/misc.h"

#ifdef _WIN32
#include <common/clean_windows.h>
#else
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string>

#include <cstdint>

namespace common {
namespace fs = std::filesystem;

// Global mutex for static variables in this module
static std::mutex& mutex() {
    static std::mutex mutex{};
    return mutex;
}

static bool& initStatus() {
    static bool init_status = false;
    return init_status;
}

bool largePagingIsEnabled() {
    std::unique_lock lock{mutex()};
    return initStatus();
}

#if defined(_WIN32)

/* Windows-specific Implementations*/

// Attempts to grant current process the SeLockMemoryPrivilege
// See run -> secpol.msc -> Local Policies/User Rights Assignment/Lock Pages in Memory to enable for a user account
// Enabled by tpcServer installer (TODO)
static bool enableLockMemoryPrivilege() {
    HANDLE hToken = NULL;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    // Get the token handle for the current process
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        message("OpenProcessToken failed: %lu", GetLastError());
        return false;
    }

    // Look up the LUID for SeLockMemoryPrivilege
    if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &luid)) {
        message("LookupPrivilegeValue failed: %lu", GetLastError());
        CloseHandle(hToken);
        return false;
    }

    // Set up the TOKEN_PRIVILEGES structure
    tp.PrivilegeCount           = 1;
    tp.Privileges[0].Luid       = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Adjust the token's privileges
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL)) {
        message("AdjustTokenPrivileges failed: %lu", GetLastError());
        CloseHandle(hToken);
        return false;
    }

    // AdjustTokenPrivileges can succeed but still fail partially
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        message("The token does not have SeLockMemoryPrivilege assigned.");
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);
    return true;
}

static bool hasLockMemoryPrivilege() {
    HANDLE hToken = NULL;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        message("OpenProcessToken failed: %lu", GetLastError());
        return false;
    }

    LUID luid;
    if (!LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &luid)) {
        message("LookupPrivilegeValue failed: %lu", GetLastError());
        CloseHandle(hToken);
        return false;
    }

    PRIVILEGE_SET ps;
    ps.PrivilegeCount          = 1;
    ps.Control                 = PRIVILEGE_SET_ALL_NECESSARY;
    ps.Privilege[0].Luid       = luid;
    ps.Privilege[0].Attributes = 0;

    BOOL result = FALSE;
    if (!PrivilegeCheck(hToken, &ps, &result)) {
        message("PrivilegeCheck failed: %lu", GetLastError());
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);
    return result != FALSE;
}

size_t sysPageSize() {
    static const size_t page_size = []() {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        return (size_t)sys_info.dwPageSize;
    }();
    return page_size;
}

size_t sysLargePageSize() {
    static const size_t large_page_size = GetLargePageMinimum();
    return large_page_size;
}

bool initLargePaging() {
    std::unique_lock lock{mutex()};

    if (sysLargePageSize() <= sysPageSize()) {
        initStatus() = false;
    }
    else if (hasLockMemoryPrivilege()) {
        initStatus() = true;
    }
    else {
        initStatus() = enableLockMemoryPrivilege();
    }
    return initStatus();
}

// Allocation size is rounded up to a multiple of system's page size if allocated_size is non-null
void* allocPages(size_t size, bool use_large_pages, size_t* allocated_size) {
    DWORD sys_flags = MEM_COMMIT | MEM_RESERVE;
    size_t rounded_size{};
    size_t page_size{};

    if (use_large_pages) {
        sys_flags |= MEM_LARGE_PAGES;
        page_size = sysLargePageSize();
    }
    else {
        page_size = sysPageSize();
    }

    rounded_size = utils::roundCeilMult(size, page_size);
    if (!allocated_size && size != rounded_size) {
        return nullptr;
    }

    void* buf = VirtualAlloc(nullptr, rounded_size, sys_flags, PAGE_READWRITE);

    if (!buf) return nullptr;

    if (allocated_size) {
        *allocated_size = rounded_size;
    }

    return buf;
}

void freePages(void* buf, [[maybe_unused]] size_t size) {
    if (buf) VirtualFree(buf, 0, MEM_RELEASE);
}

#elif defined(_LINUX)

/* Linux-specific Implementation*/

size_t sysPageSize() {
    static const size_t page_size = sysconf(_SC_PAGESIZE);
    return page_size;
}

size_t sysLargePageSize() {
    static const size_t large_page_size = []() -> size_t {
        FILE* f = std::fopen("/proc/meminfo", "r");
        if (!f) return 0;
        auto guard = utils::ScopeExit([&] { std::fclose(f); });

        size_t size_kb = 0;
        char line[256];
        while (std::fgets(line, sizeof(line), f)) {
            if (std::sscanf(line, "Hugepagesize: %zu kB", &size_kb) == 1) {
                break;
            }
        }
        return size_kb * 1024;
    }();
    return large_page_size;
}

static const fs::path kHugePagesRoot{"/sys/kernel/mm/hugepages/"};

static const fs::path& hugePagesDir() {
    static const fs::path dir = []() {
        auto size_kb       = sysLargePageSize() / 1024;
        std::string folder = "hugepages-" + std::to_string(size_kb) + "kB";
        return kHugePagesRoot / folder;
    }();
    return dir;
}

// Number of huge pages available to the system
static int hugePagesGet(void) {
    std::ifstream f = {hugePagesDir() / "nr_hugepages"};
    int count{};
    if (!(f >> count)) return -1;
    return count;
}

// Number of huge pages that are free to be mapped
static int hugePagesFree(void) {
    std::ifstream f = {hugePagesDir() / "free_hugepages"};
    int count{};
    if (!(f >> count)) return -1;
    return count;
}

// Use POSIX write() to set the number of available huge pages on system. Caller must ensure appropriate privileges.
static int hugePagesSet(int requested) {
    fs::path path = hugePagesDir() / "nr_hugepages";

    int fd = open(path.c_str(), O_WRONLY);
    if (fd == -1) {
        warning("Unable to open file %s", path.c_str());
        return -1;
    }
    auto guard = utils::ScopeExit([&] { close(fd); });

    std::string val = std::to_string(requested);
    if (write(fd, val.c_str(), val.size()) == -1) {
        warning("Failed to write to %s", path.c_str());
        return -1;
    }

    // Kernel may grant fewer than requested
    int actual = hugePagesGet();
    if (actual < requested) {
        warning("Warning: requested %d huge pages, got %d", requested, actual);
        return actual;
    }

    return actual;
}

constexpr int MB = 1024 * 1024;

const int kLargePagesRequestAmount = (64 * MB) / sysLargePageSize();
const int kLargePagesSuccessAmount = (40 * MB) / sysLargePageSize();

// We check whether the OS is configured with hugepages available. If not, we try to set the appropriate value in sysfs
// file (requires elevated privileges).
bool initLargePaging() {
    std::unique_lock lock{mutex()};

    initStatus() = false;
    int num_huge = hugePagesGet();
    if (num_huge < 0) {
        // Error reading sysfs
    }
    else if (num_huge > kLargePagesSuccessAmount) {
        initStatus() = true;
    }
    else {
        int ret = hugePagesSet(kLargePagesRequestAmount);
        if (ret >= kLargePagesSuccessAmount) {
            initStatus() = true;
        }
    }

    return initStatus();
}

// Allocation size is rounded up to a multiple of system's page size if allocated_size is non-null
void* allocPages(size_t size, bool use_large_pages, size_t* allocated_size) {
    int sys_flags = MAP_PRIVATE | MAP_ANONYMOUS;
    size_t rounded_size{};
    size_t page_size{};

    if (use_large_pages) {
        if (!largePagingIsEnabled()) {
            return nullptr;  // Please call initLargePaging()
        }
        sys_flags |= MAP_HUGETLB;
        page_size = sysLargePageSize();
    }
    else {
        page_size = sysPageSize();
    }

    rounded_size = utils::roundCeilMult(size, page_size);
    if (!allocated_size && size != rounded_size) {
        return nullptr;
    }

    void* buf = mmap(nullptr, rounded_size, PROT_READ | PROT_WRITE, sys_flags, -1, 0);
    if (buf == MAP_FAILED) {
        return nullptr;
    }

    if (allocated_size) {
        *allocated_size = rounded_size;
    }

    return buf;
}

void freePages(void* buf, size_t size) {
    if (buf) munmap(buf, size);
}

#endif  // OS Platform

LargePageArena::LargePageArena(size_t size) {
    m_mem = common::allocPages(size, true, &m_size);
    if (!m_mem) {
        throw std::runtime_error("Failed to alloc large pages");
    }
    auto mem_guard = utils::ScopeExit{[&] { common::freePages(m_mem, m_size); }};

    m_mspace = create_mspace_with_base(m_mem, m_size, 1);
    if (!m_mspace) {
        throw std::runtime_error("Error creating mspace");
    }

    mem_guard.release();
}

LargePageArena::~LargePageArena() {
    if (m_mspace) {
        destroy_mspace(m_mspace);
    }
    if (m_mem) {
        common::freePages(m_mem, m_size);
    }
}

void* LargePageArena::allocate(size_t size, size_t alignment) {
    std::unique_lock lock{m_mutex};
    return mspace_memalign(m_mspace, alignment, size);  // param order is switched!
}

void LargePageArena::deallocate(void* ptr) noexcept {
    std::unique_lock lock{m_mutex};
    mspace_free(m_mspace, ptr);
}

}  // namespace common
