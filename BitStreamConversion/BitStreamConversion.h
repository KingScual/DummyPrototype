#pragma once

#include <vector>
#include <cstdint>
#include <type_traits>
#include <cstring>

// Simple utility to convert trivially-copyable types to/from a byte-based bitstream.
// The functions operate on raw bytes (a byte-array bitstream). For arbitrary
// complex types you should provide custom serialization.
class BitStreamConversion
{
public:
    // Convert a trivially-copyable value into a byte vector representing its raw memory.
    template<typename T>
    static std::vector<uint8_t> ToBitStream(const T& value)
    {
        static_assert(std::is_trivially_copyable<T>::value, "ToBitStream requires a trivially copyable type");
        std::vector<uint8_t> out(sizeof(T));
        std::memcpy(out.data(), &value, sizeof(T));
        return out;
    }

    // Convert a byte vector (bitstream) back into a trivially-copyable value.
    // Returns true on success (when the provided buffer has at least sizeof(T) bytes).
    template<typename T>
    static bool FromBitStream(const std::vector<uint8_t>& bits, T& out)
    {
        static_assert(std::is_trivially_copyable<T>::value, "FromBitStream requires a trivially copyable type");
        if (bits.size() < sizeof(T))
            return false;
        std::memcpy(&out, bits.data(), sizeof(T));
        return true;
    }
};
