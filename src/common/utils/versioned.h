#ifndef UTILS_VERSIONED_H
#define UTILS_VERSIONED_H

#include <any>
#include <concepts>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <type_traits>

/**
 * Versioned Struct Utility with Type-Erased Container
 *
 * Usage Example:
 * --------------
 * namespace myapp {
 *     // Declare versioned structs (default starts at 0)
 *     DECLARE_VERSIONED_STRUCT(Config, 2);
 *     DECLARE_VERSIONED_STRUCT_RANGE(Settings, 5, 8);  // versions 5-8
 *
 *     // Define versions
 *     VERSIONED_STRUCT(Config, 0) {
 *         uint32_t id;
 *         uint32_t value;
 *     };
 *
 *     VERSIONED_STRUCT(Config, 1) {
 *         uint32_t id;
 *         uint32_t value;
 *         uint32_t flags;  // Added in v1
 *     };
 *
 *     VERSIONED_STRUCT(Config, 2) {
 *         uint32_t id;
 *         uint32_t value;
 *         uint32_t flags;
 *         char name[32];   // Added in v2
 *     };
 *
 *     // Define upgrade functions between consecutive versions
 *     Config<1> upgrade(const Config<0>& v0) {
 *         return Config<1>{v0.id, v0.value, 0};
 *     }
 *     Config<2> upgrade(const Config<1>& v1) {
 *         Config<2> v2{v1.id, v1.value, v1.flags, {}};
 *         strcpy(v2.name, "unnamed");
 *         return v2;
 *     }
 * }
 *
 * // Using versioned structs:
 * myapp::Config<0> old_config{1, 42};
 * auto latest = old_config.to_latest();        // Auto-upgrades to Config<2>
 * auto v1 = old_config.to_version<1>();        // Upgrade to specific version
 *
 * // Type-erased container for any version:
 * utils::AnyVersion<myapp::Config> stored = myapp::Config<1>{2, 3, 5};
 * int ver = stored.version();                  // Returns 1
 * auto latest_cfg = stored.to_latest();        // Returns Config<2>
 * auto v2 = stored.to_version<2>();            // Convert to specific version
 * Config<1>& original = utils::version_cast<1>(stored); // Access original type
 *
 * // Static assertions:
 * static_assert(myapp::Config<>::latest == 2);
 * static_assert(myapp::Config<>::min == 0);
 * static_assert(myapp::Settings<>::min == 5);
 */

/*

Enhancement ideas:
- Suppport for downgrade functions
- Configurable name instead of "upgrade"
- Use of compile-time jump-table instead of recursive templates
- Explore approach where nested template is specialized instead

*/

namespace utils {
// Concept to check if a type is versioned
template <typename T>
concept is_versioned_type = requires {
    { T::current } -> std::convertible_to<int>;
    { T::latest } -> std::convertible_to<int>;
    { T::min } -> std::convertible_to<int>;
};

// For checking template template arguments directly
template <template <int> typename T, int V = 0>
concept is_versioned = is_versioned_type<T<V>>;

// Concept to check if upgrade function exists
template <typename From, typename To>
concept has_upgrade = requires(const From& from) {
    { upgrade(from) } -> std::same_as<To>;
};

// Generic versioned base using CRTP
template <template <int> class DerivedTemplate, int latest_version, int min_version, int version>
struct Versioned {
    static constexpr int latest        = latest_version;
    static constexpr int min           = min_version;
    static constexpr int current       = version;
    static constexpr bool is_latest    = (current == latest);
    static constexpr bool is_min       = (current == min);
    static constexpr int version_count = latest - min + 1;

    using type = DerivedTemplate<version>;

    // Version checking utilities
    static constexpr bool is_version(int v) { return v == version; }
    static constexpr bool is_at_least(int v) { return version >= v; }

    // Get the type for a specific version
    template <int V>
    using versioned_type = DerivedTemplate<V>;

    // Convert to a specific version (must be >= current version)
    template <int V>
    auto to_version() const requires(V > version && V <= latest_version)
    {
        return to_version_impl<version, V>(static_cast<const DerivedTemplate<version>&>(*this));
    }

    // Already at the requested version
    template <int V>
    const DerivedTemplate<version>& to_version() const requires(V == version)
    {
        return static_cast<const DerivedTemplate<version>&>(*this);
    }

    // Non-const versions
    template <int V>
    auto to_version() requires(V > version && V <= latest_version)
    {
        return to_version_impl<version, V>(static_cast<const DerivedTemplate<version>&>(*this));
    }

    template <int V>
    DerivedTemplate<version>& to_version() requires(V == version)
    {
        return static_cast<DerivedTemplate<version>&>(*this);
    }

    // to_latest method - converts current version to latest
    auto to_latest() const requires(!is_latest)
    {
        return to_version_impl<version, latest_version>(static_cast<const DerivedTemplate<version>&>(*this));
    }

    // Already at latest version
    const DerivedTemplate<version>& to_latest() const requires(is_latest)
    {
        return static_cast<const DerivedTemplate<version>&>(*this);
    }

    // Non-const versions
    auto to_latest() requires(!is_latest)
    {
        return to_version_impl<version, latest_version>(static_cast<const DerivedTemplate<version>&>(*this));
    }

    DerivedTemplate<version>& to_latest() requires(is_latest)
    {
        return static_cast<DerivedTemplate<version>&>(*this);
    }

    bool operator==(const Versioned& other) const = default;

   private:
    // Recursive implementation of to_version
    template <int CurrentV, int TargetV>
    requires(TargetV > CurrentV)
    static auto to_version_impl(const DerivedTemplate<CurrentV>& obj) {
        if constexpr (CurrentV == TargetV) {
            // Base case: reached target version
            return obj;
        }
        else {
            // Recursive case: upgrade one version and continue
            using CurrentType = DerivedTemplate<CurrentV>;
            using NextType    = DerivedTemplate<CurrentV + 1>;

            static_assert(has_upgrade<CurrentType, NextType>,
                          "Missing upgrade function for version transition. "
                          "Please define: NextType upgrade(const CurrentType&)");

            auto upgraded = upgrade(obj);

            if constexpr (CurrentV + 1 == TargetV) {
                // We've reached the target version
                return upgraded;
            }
            else {
                // Continue upgrading
                return to_version_impl<CurrentV + 1, TargetV>(upgraded);
            }
        }
    }
};

template <template <int> class VersionedType>
requires is_versioned<VersionedType>
class AnyVersion;

// Forward declarations for friend functions
template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
const VersionedType<V>& version_cast(const AnyVersion<VersionedType>& operand);

template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
VersionedType<V>& version_cast(AnyVersion<VersionedType>& operand);

template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
VersionedType<V> version_cast(AnyVersion<VersionedType>&& operand);

template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
const VersionedType<V>* version_cast(const AnyVersion<VersionedType>* operand) noexcept;

template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
VersionedType<V>* version_cast(AnyVersion<VersionedType>* operand) noexcept;

template <template <int> class VersionedType>
requires is_versioned<VersionedType>
class AnyVersion {
   private:
    static constexpr int min_version    = VersionedType<0>::min;
    static constexpr int latest_version = VersionedType<0>::latest;
    static constexpr int version_count  = latest_version - min_version + 1;
    static constexpr int reset_val      = std::numeric_limits<int>::min();

    std::any m_storage;
    int m_stored_version = reset_val;

    // Helper to check if a type is a valid version of our versioned type
    template <typename T>
    static constexpr bool is_valid_version() {
        if constexpr (is_versioned_type<T>) {
            constexpr int current = T::current;
            return std::is_same_v<T, VersionedType<current>>;
        }
        return false;
    }

    // Iterative helper to find and upgrade from current version
    template <int TargetV, int CurrentV = min_version>
    VersionedType<TargetV> find_and_upgrade() const {
        if constexpr (!requires { std::declval<const VersionedType<CurrentV>&>().template to_version<TargetV>(); }) {
            throw std::runtime_error("Invalid version upgrade");
        }
        else {
            if (m_stored_version == CurrentV) {
                // Found our current version, now upgrade to target
                if constexpr (TargetV == CurrentV) {
                    return std::any_cast<const VersionedType<CurrentV>&>(m_storage);
                }
                else {
                    return std::any_cast<const VersionedType<CurrentV>&>(m_storage).template to_version<TargetV>();
                }
            }
            else {
                // Keep searching for current version
                return find_and_upgrade<TargetV, CurrentV + 1>();
            }
        }
    }

   public:
    // Default constructor
    AnyVersion() noexcept = default;

    // Single constructor with forwarding reference
    template <typename T>
    requires std::is_same_v<std::decay_t<T>, VersionedType<std::decay_t<T>::current>>
    AnyVersion(T&& value) : m_storage(std::forward<T>(value)),
                            m_stored_version(std::decay_t<T>::current) {}

    // Single assignment with forwarding reference
    template <typename T>
    requires std::is_same_v<std::decay_t<T>, VersionedType<std::decay_t<T>::current>>
    AnyVersion& operator=(T&& value) {
        m_storage        = std::forward<T>(value);
        m_stored_version = std::decay_t<T>::current;
        return *this;
    }

    AnyVersion(const AnyVersion& other) : m_storage(other.m_storage), m_stored_version(other.m_stored_version) {}
    AnyVersion(AnyVersion&& other) noexcept
        : m_storage(std::move(other.m_storage)),
          m_stored_version(other.m_stored_version) {}

    AnyVersion& operator=(const AnyVersion& other) {
        if (this != &other) {
            m_storage        = other.m_storage;
            m_stored_version = other.m_stored_version;
        }
        return *this;
    }
    AnyVersion& operator=(AnyVersion&& other) noexcept {
        if (this != &other) {
            m_storage        = std::move(other.m_storage);
            m_stored_version = other.m_stored_version;
        }
        return *this;
    }

    // Friend declarations for version_cast functions
    template <int V, template <int> class VT>
    requires is_versioned<VT>
    friend const VT<V>& version_cast(const AnyVersion<VT>& operand);

    template <int V, template <int> class VT>
    requires is_versioned<VT>
    friend VT<V>& version_cast(AnyVersion<VT>& operand);

    template <int V, template <int> class VT>
    requires is_versioned<VT>
    friend VT<V> version_cast(AnyVersion<VT>&& operand);

    template <int V, template <int> class VT>
    requires is_versioned<VT>
    friend const VT<V>* version_cast(const AnyVersion<VT>* operand) noexcept;

    template <int V, template <int> class VT>
    requires is_versioned<VT>
    friend VT<V>* version_cast(AnyVersion<VT>* operand) noexcept;

    // Reset to empty state
    void reset() noexcept {
        m_storage.reset();
        m_stored_version = reset_val;
    }

    // Check if contains a value
    bool has_value() const noexcept { return m_storage.has_value(); }

    // Get type info of stored value
    const std::type_info& type() const noexcept { return m_storage.type(); }

    // Get the stored version number (throws if empty)
    int version() const {
        if (!has_value()) {
            throw std::runtime_error("AnyVersion is empty");
        }
        return m_stored_version;
    }

    // Safe version check - returns optional
    std::optional<int> version_if() const noexcept {
        if (!has_value()) {
            return std::nullopt;
        }
        return m_stored_version;
    }

    // Check if storing latest version
    bool is_latest() const {
        if (!has_value()) {
            throw std::runtime_error("AnyVersion is empty");
        }
        return m_stored_version == latest_version;
    }

    // Check if storing minimum version
    bool is_min() const {
        if (!has_value()) {
            throw std::runtime_error("AnyVersion is empty");
        }
        return m_stored_version == min_version;
    }

    // Get as latest version (automatically upgrades if needed)
    VersionedType<latest_version> to_latest() const {
        if (!has_value()) {
            throw std::runtime_error("AnyVersion is empty");
        }
        return find_and_upgrade<latest_version>();
    }

    // Convert to a specific version (must be >= current version)
    template <int V>
    requires(V >= min_version && V <= latest_version)
    VersionedType<V> to_version() const {
        if (!has_value()) {
            throw std::runtime_error("AnyVersion is empty");
        }

        const int current = m_stored_version;

        if (V < current) {
            throw std::runtime_error("Cannot downgrade from version " + std::to_string(current) + " to " +
                                     std::to_string(V));
        }

        return find_and_upgrade<V>();
    }

    // Emplace a new value
    template <int V, typename... Args>
    requires(V >= min_version && V <= latest_version)
    void emplace(Args&&... args) {
        m_storage.emplace<VersionedType<V>>(std::forward<Args>(args)...);
        m_stored_version = V;
    }

    // Swap with another AnyVersion
    void swap(AnyVersion& other) noexcept {
        m_storage.swap(other.m_storage);
        std::swap(m_stored_version, other.m_stored_version);
    }
};

// Implementation of version_cast functions

// Direct cast - throws if wrong version
template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
const VersionedType<V>& version_cast(const AnyVersion<VersionedType>& operand) {
    if (!operand.m_storage.has_value()) {
        throw std::runtime_error("AnyVersion is empty");
    }
    if (operand.m_stored_version != V) {
        throw std::runtime_error("Version mismatch: stored version " + std::to_string(operand.m_stored_version) +
                                 ", requested version " + std::to_string(V));
    }
    return std::any_cast<const VersionedType<V>&>(operand.m_storage);
}

template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
VersionedType<V>& version_cast(AnyVersion<VersionedType>& operand) {
    if (!operand.m_storage.has_value()) {
        throw std::runtime_error("AnyVersion is empty");
    }
    if (operand.m_stored_version != V) {
        throw std::runtime_error("Version mismatch: stored version " + std::to_string(operand.m_stored_version) +
                                 ", requested version " + std::to_string(V));
    }
    return std::any_cast<VersionedType<V>&>(operand.m_storage);
}

// Rvalue reference version - moves the value out of AnyVersion
template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
VersionedType<V> version_cast(AnyVersion<VersionedType>&& operand) {
    if (!operand.m_storage.has_value()) {
        throw std::runtime_error("AnyVersion is empty");
    }
    if (operand.m_stored_version != V) {
        throw std::runtime_error("Version mismatch: stored version " + std::to_string(operand.m_stored_version) +
                                 ", requested version " + std::to_string(V));
    }
    return std::any_cast<VersionedType<V>>(std::move(operand.m_storage));
}

// Safe cast - returns pointer
template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
const VersionedType<V>* version_cast(const AnyVersion<VersionedType>* operand) noexcept {
    if (!operand || !operand->m_storage.has_value() || operand->m_stored_version != V) {
        return nullptr;
    }
    return std::any_cast<VersionedType<V>>(&operand->m_storage);
}

template <int V, template <int> class VersionedType>
requires is_versioned<VersionedType>
VersionedType<V>* version_cast(AnyVersion<VersionedType>* operand) noexcept {
    if (!operand || !operand->m_storage.has_value() || operand->m_stored_version != V) {
        return nullptr;
    }
    return std::any_cast<VersionedType<V>>(&operand->m_storage);
}
}  // namespace utils

namespace std {
template <template <int> class VersionedType>
requires utils::is_versioned<VersionedType>
void swap(utils::AnyVersion<VersionedType>& lhs, utils::AnyVersion<VersionedType>& rhs) noexcept {
    lhs.swap(rhs);
}
}  // namespace std

// Utility macros to simplify declaration

// Default macro for min version = 0
#define DECLARE_VERSIONED_STRUCT(Name, Latest)           \
    template <int version = Latest>                      \
    struct Name;                                         \
                                                         \
    inline constexpr int Name##_latest_version = Latest; \
    inline constexpr int Name##_min_version    = 0;      \
                                                         \
    template <int version>                               \
    struct Name : ::utils::Versioned<Name, Latest, 0, version> {}

// Explicit macro when you need a custom min version
#define DECLARE_VERSIONED_STRUCT_RANGE(Name, Min, Latest) \
    template <int version = Latest>                       \
    struct Name;                                          \
                                                          \
    inline constexpr int Name##_latest_version = Latest;  \
    inline constexpr int Name##_min_version    = Min;     \
                                                          \
    template <int version>                                \
    struct Name : ::utils::Versioned<Name, Latest, Min, version> {}

// Macro for defining specific versions
#define VERSIONED_STRUCT(Name, Version) \
    template <>                         \
    struct Name<Version> : ::utils::Versioned<Name, Name##_latest_version, Name##_min_version, Version>

#endif  // UTILS_VERSIONED_H