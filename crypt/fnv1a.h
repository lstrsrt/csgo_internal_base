#pragma once

using hash_t = uint_fast32_t;

inline namespace crypt {

    namespace fnv1a {

        constexpr hash_t basis = 0x811c9dc5;
        constexpr hash_t prime = 0x1000193;

        template<std::integral ch = char>
        consteval hash_t ct(const ch* str, hash_t val = basis) noexcept
        {
            if (str[0] == '\0')
                return val;
            else
                return ct(&str[1], (val ^ static_cast<hash_t>(str[0])) * prime);
        }

        template<std::integral ch = char>
        inline hash_t rt(const ch* str) noexcept
        {
            const auto len = [str]() {
                size_t i{ };
                while (str[i])
                    i++;
                return i;
            }();

            auto hashed = basis;
            for (size_t i = 0; i < len; i++) {
                hashed ^= str[i];
                hashed *= prime;
            }
            return hashed;
        }

        template<string_like st = std::string_view>
        inline hash_t rt(const st& str) noexcept
        {
            return rt(str.data());
        }

    }

}
