#ifndef UTILS_HASH_MAP_H
#define UTILS_HASH_MAP_H

#include "unordered_dense.h"

// Convenience header defining our own wrappers for ankerl::unordered_dense
// Use as a drop-in replacement for std::unordered_map with better performance in most use-cases
//
// Key differences from std::unordered_map:
// - operator[] is DELETED to prevent accidental insertion of default values
// - Use .at() for access that throws std::out_of_range if key is missing
// - Use .find() for explicitly handled lookup
// - Use .add() for insertion with designated initializers support
// - Use .insert_or_assign() or .try_emplace() for other insertion/update patterns
//
// Example usage:
//   utils::HashMap<std::string, int> map;
//
//   // Insert:
//   map.add("key", 42);                          // Simple insert
//   map.add("key2", {.field1 = 1, .field2 = 2}); // With designated initializers
//   map.try_emplace("key3", 100);                // In-place construction
//
//   // Insert or update:
//   map.insert_or_assign("key", 99);
//
//   // Update existing (throws if missing):
//   map.at("key") = 50;
//   map.at("key2") = {.field1 = 9, .field2 = 8};
//
//   // Read with exception on missing key:
//   int value = map.at("key");
//
//   // Read with explicit check (preferred for optional keys):
//   if (auto it = map.find("key"); it != map.end()) {
//       int value = it->second;
//   }
//
//   // This will NOT compile (operator[] is deleted):
//   // map["key"] = 42;  // ERROR: forces explicit choice between insert/update

namespace utils {

template <class Key, class T, class Hash = ankerl::unordered_dense::hash<Key>, class KeyEqual = std::equal_to<Key>,
          class AllocatorOrContainer = std::allocator<std::pair<Key, T>>>
class HashMap : public ankerl::unordered_dense::map<Key, T, Hash, KeyEqual, AllocatorOrContainer> {
   public:
    using Base = ankerl::unordered_dense::map<Key, T, Hash, KeyEqual, AllocatorOrContainer>;
    using Base::Base;  // Inherit all constructors

    // Add/insert a new key-value pair (does not update if key exists).
    // Supports designated initializers: map.add(key, {.field1 = ..., .field2 = ...})
    // Returns pair<iterator, bool> where bool is true if insertion occurred.
    template <typename K> auto add(K&& key, T&& value) -> std::pair<typename Base::iterator, bool> {
        return this->emplace(std::forward<K>(key), std::forward<T>(value));
    }

    // Add/insert a new key-value pair with in-place construction (does not update if key exists).
    // Constructs value from constructor arguments: map.add(key, arg1, arg2, ...)
    // Returns pair<iterator, bool> where bool is true if insertion occurred.
    template <typename K, typename... Args>
    auto add(K&& key, Args&&... args) -> std::pair<typename Base::iterator, bool> {
        return this->try_emplace(std::forward<K>(key), std::forward<Args>(args)...);
    }

    // Delete operator[] to prevent accidental insertion of default values.
    // Use .at() for access, .add() for insert, or .insert_or_assign() for insert-or-update.
    T& operator[](const Key& key) = delete;
    T& operator[](Key&& key)      = delete;
};

template <class Key, class T, class Hash = ankerl::unordered_dense::hash<Key>, class KeyEqual = std::equal_to<Key>,
          class AllocatorOrContainer = std::allocator<std::pair<Key, T>>>
class SegmentedHashMap : public ankerl::unordered_dense::segmented_map<Key, T, Hash, KeyEqual, AllocatorOrContainer> {
   public:
    using Base = ankerl::unordered_dense::segmented_map<Key, T, Hash, KeyEqual, AllocatorOrContainer>;
    using Base::Base;  // Inherit all constructors

    // Add/insert a new key-value pair (does not update if key exists).
    // Supports designated initializers: map.add(key, {.field1 = ..., .field2 = ...})
    // Returns pair<iterator, bool> where bool is true if insertion occurred.
    template <typename K> auto add(K&& key, T&& value) -> std::pair<typename Base::iterator, bool> {
        return this->emplace(std::forward<K>(key), std::forward<T>(value));
    }

    // Add/insert a new key-value pair with in-place construction (does not update if key exists).
    // Constructs value from constructor arguments: map.add(key, arg1, arg2, ...)
    // Returns pair<iterator, bool> where bool is true if insertion occurred.
    template <typename K, typename... Args>
    auto add(K&& key, Args&&... args) -> std::pair<typename Base::iterator, bool> {
        return this->try_emplace(std::forward<K>(key), std::forward<Args>(args)...);
    }

    // Delete operator[] to prevent accidental insertion of default values.
    // Use .at() for access, .add() for insert, or .insert_or_assign() for insert-or-update.
    T& operator[](const Key& key) = delete;
    T& operator[](Key&& key)      = delete;
};

// HashSet and SegmentedHashSet don't have operator[], so they remain as simple aliases
template <class Key, class Hash = ankerl::unordered_dense::hash<Key>, class KeyEqual = std::equal_to<Key>,
          class AllocatorOrContainer = std::allocator<Key>>
using HashSet = ankerl::unordered_dense::set<Key, Hash, KeyEqual, AllocatorOrContainer>;

template <class Key, class Hash = ankerl::unordered_dense::hash<Key>, class KeyEqual = std::equal_to<Key>,
          class AllocatorOrContainer = std::allocator<Key>>
using SegmentedHashSet = ankerl::unordered_dense::segmented_set<Key, Hash, KeyEqual, AllocatorOrContainer>;

}  // namespace utils

#endif  // UTILS_HASH_MAP_H
