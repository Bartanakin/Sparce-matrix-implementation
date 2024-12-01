#pragma once
#include <iostream>

namespace Barta {
template<typename T>
struct Triplet {
    unsigned int row;
    unsigned int col;
    T val;

    Triplet() noexcept = default;
    Triplet(
        unsigned int row,
        unsigned int col,
        T val
    ) noexcept:
        row(row),
        col(col),
        val(val) {}

    static void sort(
        std::vector<Triplet>& triplets
    ) {
        std::sort(triplets.begin(), triplets.end(), [](const Triplet<T>& l, const Triplet<T>& r) {
            if (l.row != r.row) {
                return l.row < r.row;
            }
            return l.col < r.col;
        });
    }

    static std::vector<Triplet> mergeSorted(const std::vector<Triplet>& triplets) {
        std::vector<Triplet> mergedTriplets;

        mergedTriplets.reserve(triplets.size());
        if (!triplets.empty()) {
            mergedTriplets.push_back(triplets[0]);
        }

        int cur = 0;
        for (int i = 1; i < triplets.size(); i++) {
            if (mergedTriplets[cur].row == triplets[i].row && mergedTriplets[cur].col == triplets[i].col) {
                mergedTriplets[cur].val += triplets[i].val;
            } else {
                cur++;
                mergedTriplets.push_back(triplets[i]);
            }
        }

        return mergedTriplets;
    }
};
}
