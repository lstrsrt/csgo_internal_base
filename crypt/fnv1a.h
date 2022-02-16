#pragma once

using hash_t = uint_fast32_t;

inline namespace crypt {

    namespace fnv1a {

        constexpr hash_t basis = 0x811c9dc5;
        constexpr hash_t prime = 0x1000193;

        consteval hash_t ct(const char* str, hash_t val = basis) noexcept
        {
            if (str[0] == '\0')
                return val;
            else
                return ct(&str[1], (val ^ static_cast<hash_t>(str[0])) * prime);
        }

        inline const hash_t rt(const char* str) noexcept
        {
            auto hashed = basis;
            for (size_t i = 0; i < strlen(str); i++) {
                hashed ^= str[i];
                hashed *= prime;
            }
            return hashed;
        }

        inline const hash_t rt(std::string_view str) noexcept
        {
            return rt(str.data());
        }

    }

}
