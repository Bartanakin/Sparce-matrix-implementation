#pragma once

namespace Barta {
    template<typename T>
    struct Triplet {
        unsigned int row;
        unsigned int col;
        T val;

        Triplet(
            unsigned int row,
            unsigned int col,
            T val
        ) :
            row(row),
            col(col),
            val(val)
        {}
    };
}
