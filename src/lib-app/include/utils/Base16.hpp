#pragma once

#include <algorithm>
#include <execution>
#include <ranges>
#include <string>

class Base16 final
{
public:
    static inline constexpr const char GLYPHS[] = "0123456789ABCDEF";

    static std::string toBase16(const std::string& str)
    {
        std::string result = "";
        result.resize(str.size() * 2);
        auto indices = std::views::iota(size_t { 0 }, str.size());
        std::for_each(
            std::execution::par_unseq,
            indices.begin(),
            indices.end(),
            [&](size_t idx)
            {
                const auto cast = static_cast<unsigned char>(str[idx]);
                const auto idx1 = (cast & 0xf0) >> 4;
                result[2 * idx] = static_cast<char>('a') + idx1;
                const auto idx2 = static_cast<unsigned char>(str[idx]) & 0x0f;
                result[2 * idx + 1] = static_cast<char>('a') + idx2;
            });
        return result;
    }

    static std::string fromBase16(const std::string& str)
    {
        if (str.size() % 2 == 1) throw std::runtime_error("Str has odd size");
        std::string result = "";
        result.resize(str.size() / 2);

        for (size_t idx = 0; idx < result.size(); ++idx)
        {
            result[idx] = (static_cast<unsigned char>(str[2 * idx]) - 'a') << 4;
            result[idx] |= (static_cast<unsigned char>(str[2 * idx + 1]) - 'a');
        }

        return result;
    }
};