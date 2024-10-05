#pragma once
#include <algorithm>

namespace Barta {
    template <typename T>
    concept NumericType = (std::integral<T> || std::floating_point<T>) && requires {
        static_cast<T>(0);
    };
}
