#ifndef UTILS_CONTAINERS_H
#define UTILS_CONTAINERS_H

#include <algorithm>
#include <optional>
#include <type_traits>
#include <vector>

// Generic utilities for working with STL containers

namespace utils {

//=============================================================================
// Concepts
//=============================================================================

// Concept to detect if a container has a member .find() function
template <typename Container, typename T>
concept HasMemberFind = requires(Container& c, const T& value) {
    { c.find(value) } -> std::same_as<typename Container::iterator>;
};

template <typename Container, typename T>
concept HasConstMemberFind = requires(const Container& c, const T& value) {
    { c.find(value) } -> std::same_as<typename Container::const_iterator>;
};

//=============================================================================
// Container Modification Utilities
//=============================================================================

// Insert an item into vector at a specified position, resizes the vector if required
template <typename T> requires std::is_default_constructible_v<T>
void sparseInsert(std::vector<T>& vec, size_t pos, const T& value) {
    if (vec.size() <= pos) {
        vec.resize(pos + 1);
    }
    vec[pos] = value;
}

// Insert an item into vector at a specified position, resizes the vector if required
template <typename T> requires std::is_default_constructible_v<T>
void sparseInsert(std::vector<T>& vec, size_t pos, T&& value) {
    if (vec.size() <= pos) {
        vec.resize(pos + 1);
    }
    vec[pos] = std::move(value);
}

//=============================================================================
// Iterator Utilities
//=============================================================================

// Compute index from a random access iterator of an STL container
template <typename Container> requires std::random_access_iterator<typename Container::iterator>
size_t getIndex(const Container& container, typename Container::const_iterator it) {
    return it - container.begin();
}

//=============================================================================
// Search Utilities
//=============================================================================

// Find element by value in container, returns iterator
// Only enabled for containers WITHOUT member .find() (use member .find() for hash maps/sets!)
template <typename Container, typename T> requires(!HasMemberFind<Container, T>)
typename Container::iterator find(Container& container, const T& value) {
    return std::find(container.begin(), container.end(), value);
}

// Find element by value in container, returns const_iterator
// Only enabled for containers WITHOUT member .find() (use member .find() for hash maps/sets!)
template <typename Container, typename T> requires(!HasConstMemberFind<Container, T>)
typename Container::const_iterator find(const Container& container, const T& value) {
    return std::find(container.begin(), container.end(), value);
}

// Find element by predicate in container, returns iterator
template <typename Container, typename Predicate>
typename Container::iterator findIf(Container& container, Predicate pred) {
    return std::find_if(container.begin(), container.end(), pred);
}

// Find element by predicate in container, returns const_iterator
template <typename Container, typename Predicate>
typename Container::const_iterator findIf(const Container& container, Predicate pred) {
    return std::find_if(container.begin(), container.end(), pred);
}

// Check whether STL-iterator points to a found element (i.e. is not equal to container.end())
template <typename Container> bool found(const Container& container, typename Container::const_iterator it) {
    return it != container.end();
}

// Check whether STL-iterator points to a found element (i.e. is not equal to container.end())
template <typename Container> bool found(const Container& container, typename Container::iterator it) {
    return it != container.end();
}

}  // namespace utils

#endif  // UTILS_CONTAINERS_H
