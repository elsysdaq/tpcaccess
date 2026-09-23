#ifndef UTILS_BITFIELDS_H
#define UTILS_BITFIELDS_H

#include <concepts>
#include <new>
#include <type_traits>

#include <cassert>
#include <stdint.h>

/**
 * Zero-overhead bitfield abstraction using CRTP.
 *
 * Usage Example:
 * --------------
 *   struct MyReg : public utils::Bitfield<MyReg, uint32_t> {
 *       MyReg() = default;
 *       MyReg(uint32_t val) { data = val; }
 *       union {
 *           uint32_t data;          // The underlying storage (must be named 'data')
 *           Field<0, 3> status;     // Bits 0-2 (3 bits)
 *           Field<3, 1> enabled;    // Bit 3 (1 bit)
 *           Field<4, 4> channel;    // Bits 4-7 (4 bits)
 *       };
 *   };
 *
 *   MyReg reg{0x0f};             // Or MyReg reg = 0x0f
 *   reg.status = 5;              // Write to bits 0-2
 *   uint32_t s = reg.status;     // Read from bits 0-2
 *   uint32_t raw = reg;          // Implicit conversion to underlying type
 *   reg |= 0xff;                 // Bitwise operations supported
 *
 * Requirements:
 *   - Derived class must contain a union with a 'data' member of type T
 *   - sizeof(Derived) must equal sizeof(T) (no members outside the union)
 *   - Derived must be standard layout (no virtual functions/bases)
 *   - Field offset + size must not exceed sizeof(T) * 8
 */

namespace utils {

template <typename D, typename T>
concept HasTypedDataMember = requires(D d, D const cd) {
    { d.data } -> std::same_as<T&>;
    { cd.data } -> std::same_as<T const&>;
};

template <typename D, typename T>
concept MatchesSizeOf = (sizeof(D) == sizeof(T));

template <typename D, typename T>
concept ValidBitfield = HasTypedDataMember<D, T> && MatchesSizeOf<D, T> && std::is_standard_layout_v<D>;

template <uint32_t offset, uint32_t size, typename T>
concept ValidField = (size > 0 && offset + size <= sizeof(T) * 8);

template <uint32_t offset, uint32_t size, std::unsigned_integral T> constexpr T computeMask() {
    static_assert(offset + size <= sizeof(T) * 8);
    if constexpr (size == sizeof(T) * 8) {
        return ~T{0};
    }
    else {
        return ((T{1} << size) - 1) << offset;
    }
}

template <uint32_t offset, uint32_t size, std::unsigned_integral T> constexpr T getBitfield(T data) {
    using BaseT = std::remove_cv_t<T>;
    static_assert(offset + size <= sizeof(T) * 8);
    constexpr auto mask = computeMask<offset, size, BaseT>();
    return (data & mask) >> offset;
}

template <uint32_t offset, uint32_t size, std::unsigned_integral T, std::unsigned_integral U>
constexpr void setBitfield(T& data, U value) {
    using BaseT = std::remove_cv_t<T>;
    static_assert(sizeof(U) <= sizeof(T));
    static_assert(offset + size <= sizeof(T) * 8);
    constexpr auto mask = computeMask<offset, size, BaseT>();

    data = (data & ~mask) | ((value << offset) & mask);
}

// CRTP base class for type-safe bitfield structs. Derived class must contain a union
// with a 'data' member of type T and Field<> members. Provides implicit conversion
// to T and bitwise operators (|=, &=, ^=).
template <typename Derived, std::unsigned_integral T> struct Bitfield {
    template <uint32_t offset, uint32_t size> struct Field {
        static constexpr uint32_t bit_offset = offset;
        static constexpr uint32_t bit_size   = size;
        static constexpr T mask              = computeMask<offset, size, T>();

        constexpr operator T() {
            validate();
            return getBitfield<offset, size>(reinterpret_cast<Derived*>(this)->data);
        }

        constexpr operator T() const {
            validate();
            return getBitfield<offset, size>(reinterpret_cast<Derived const*>(this)->data);
        }

        constexpr operator T() volatile {
            validate();
            return getBitfield<offset, size>(reinterpret_cast<volatile Derived*>(this)->data);
        }

        constexpr operator T() const volatile {
            validate();
            return getBitfield<offset, size>(reinterpret_cast<volatile Derived const*>(this)->data);
        }

        constexpr Field& operator=(T value) {
            validate();
            if constexpr (size < sizeof(T) * 8) {
                assert(value < (T{1} << size) && "Value too large for field");
            }
            setBitfield<offset, size>(reinterpret_cast<Derived*>(this)->data, value);
            return *this;
        }

        constexpr void operator=(T value) volatile {
            validate();
            if constexpr (size < sizeof(T) * 8) {
                assert(value < (T{1} << size) && "Value too large for field");
            }
            setBitfield<offset, size>(reinterpret_cast<volatile Derived*>(this)->data, value);
        }

        constexpr Field& operator++() { return *this = T(*this) + 1; }
        constexpr Field& operator--() { return *this = T(*this) - 1; }
        constexpr Field& operator+=(T value) { return *this = T(*this) + value; }
        constexpr Field& operator-=(T value) { return *this = T(*this) - value; }
        constexpr Field& operator|=(T value) { return *this = T(*this) | value; }
        constexpr Field& operator&=(T value) { return *this = T(*this) & value; }
        constexpr Field& operator^=(T value) { return *this = T(*this) ^ value; }

        constexpr void operator++() volatile { *this = T(*this) + 1; }
        constexpr void operator--() volatile { *this = T(*this) - 1; }
        constexpr void operator+=(T value) volatile { *this = T(*this) + value; }
        constexpr void operator-=(T value) volatile { *this = T(*this) - value; }
        constexpr void operator|=(T value) volatile { *this = T(*this) | value; }
        constexpr void operator&=(T value) volatile { *this = T(*this) & value; }
        constexpr void operator^=(T value) volatile { *this = T(*this) ^ value; }

       private:
        // Static asserts to ensure that Bitfield type has been declared correctly
        constexpr static void validate() {
            static_assert(ValidField<offset, size, T>,
                          "Offset + size must be equal to or smaller than size of underlying bitfield type");
            static_assert(HasTypedDataMember<Derived, T>, "Derived class must have a 'data' member");
            static_assert(
                MatchesSizeOf<Derived, T>,
                "sizeof(Derived) must equal sizeof(T) - ensure you're using a union with no other members outside it");
            static_assert(std::is_standard_layout_v<Derived>,
                          "Derived class must be standard layout for well-defined behavior");
        }
    };

    constexpr operator T() {
        static_assert(HasTypedDataMember<Derived, T>, "Derived class must have a 'data' member of type T");
        return static_cast<Derived*>(this)->data;
    }

    constexpr operator T() const {
        static_assert(HasTypedDataMember<Derived, T>, "Derived class must have a 'data' member of type T");
        return static_cast<Derived const*>(this)->data;
    }

    constexpr operator T() volatile {
        static_assert(HasTypedDataMember<Derived, T>, "Derived class must have a 'data' member of type T");
        return static_cast<volatile Derived*>(this)->data;
    }

    constexpr operator T() const volatile {
        static_assert(HasTypedDataMember<Derived, T>, "Derived class must have a 'data' member of type T");
        return static_cast<volatile Derived const*>(this)->data;
    }

    // Bitwise operations on the whole register
    constexpr Derived& operator|=(T value) {
        static_cast<Derived*>(this)->data |= value;
        return *static_cast<Derived*>(this);
    }

    constexpr void operator|=(T value) volatile { static_cast<volatile Derived*>(this)->data |= value; }

    constexpr Derived& operator&=(T value) {
        static_cast<Derived*>(this)->data &= value;
        return *static_cast<Derived*>(this);
    }

    constexpr void operator&=(T value) volatile { static_cast<volatile Derived*>(this)->data &= value; }

    constexpr Derived& operator^=(T value) {
        static_cast<Derived*>(this)->data ^= value;
        return *static_cast<Derived*>(this);
    }

    constexpr void operator^=(T value) volatile { static_cast<volatile Derived*>(this)->data ^= value; }

   protected:
    Bitfield() = default;
};

}  // namespace utils

#endif  // UTILS_BITFIELDS_H
