#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include "NumericTypeConcept.h"
#include "Triplet.h"

namespace Barta {

    template<NumericType T>
    class SparseRowWiseMatrix {
        unsigned int width;
        unsigned int height;

        std::vector<T> values;
        std::vector<unsigned int> columnIndices;
        std::vector<unsigned int> offsets;
    public:
        using VectorType = std::vector<T>;
        using Triplet = Triplet<T>;

        SparseRowWiseMatrix(
            unsigned int width,
            unsigned int height
        ) :
            width(width),
            height(height),
            offsets(height + 1, 0) {
        }

        SparseRowWiseMatrix(
            unsigned int width,
            unsigned int height,
            std::vector<Triplet> triplets
        ) : SparseRowWiseMatrix(width, height) {
            std::sort(
                triplets.begin(),
                triplets.end(),
                [](const Triplet &l, const Triplet &r) {
                    if (l.row != r.row) {
                        return l.row < r.row;
                    }
                    return l.col < r.col;
                }
            );

            std::vector<Triplet> mergedTriplets;
            mergedTriplets.reserve(triplets.size());
            if (!triplets.empty()) {
                mergedTriplets.push_back(triplets[0]);
            }

            int cur = 0;
            for(int i = 1; i < triplets.size(); i++) {
                if (
                    mergedTriplets[cur].row == triplets[i].row
                    && mergedTriplets[cur].col == triplets[i].col
                ) {
                    mergedTriplets[cur].val += triplets[i].val;
                } else {
                    cur++;
                    mergedTriplets.push_back(triplets[i]);
                }
            }

            this->values.resize(mergedTriplets.size(), static_cast<T>(0));
            this->columnIndices.resize(mergedTriplets.size(), static_cast<T>(0));
            auto rowSizes = std::vector<std::size_t>(this->height);
            unsigned int i = 0;
            for (const auto triplet: mergedTriplets) {
                this->values[i] = triplet.val;
                this->columnIndices[i] = triplet.col;
                this->offsets[triplet.row]++;

                i++;
            }

            unsigned int savedOffset = this->values.size();
            for (unsigned int i = this->offsets.size() - 1; i >= 1; i--) {
                this->offsets[i] = savedOffset;
                savedOffset -= this->offsets[i - 1];
            }

            this->offsets[0] = savedOffset;

            if (savedOffset != 0) {
                throw std::runtime_error("offset is different than 0!");
            }
        }

        void insert(unsigned int row, unsigned int col, T value) {
            unsigned int newPos;
            for (newPos = this->offsets[row]; newPos < this->offsets[row + 1]; newPos++) {
                if (col == this->columnIndices[newPos]) {
                    this->values[newPos] = value;

                    return;
                }

                if (col < this->columnIndices[newPos]) {
                    break;
                }
            }

            for (size_t i = row + 1; i < this->offsets.size(); i++) {
                this->offsets[i]++;
            }

            this->columnIndices.insert(this->columnIndices.begin() + newPos, col);
            this->values.insert(this->values.begin() + newPos, value);
        }

        VectorType operator*(const VectorType &v) const {
            assert(this->height == v.size());

            auto ret = VectorType(v.size(), static_cast<T>(0));
            auto row = 0;
            for (int i = 0; i < this->values.size(); i++) {
                while (this->offsets[row + 1] <= i) {
                    row++;
                }

                ret[row] += this->values[i] * v[this->columnIndices[i]];
            }

            return ret;
        }

        std::string toString() const {
            std::stringstream ss;
            constexpr const unsigned int w = 6;
            ss << "[";
            unsigned int cursor = 0;
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    ss << std::setw(w);
                    if (this->offsets[i + 1] > cursor && this->columnIndices[cursor] == j) {
                        ss << this->values[cursor];
                        cursor++;
                    } else {
                        ss << "0";
                    }
                }

                if (i == this->height - 1) {
                    ss << "]";
                }

                ss << std::endl;
            }

            return ss.str();
        }
    };

    template<NumericType T>
    std::ostream &operator<<(std::ostream &s, const SparseRowWiseMatrix<T> &mat) {
        constexpr const unsigned int w = 6;
        s << "[";
        unsigned int cursor = 0;
        for (int i = 0; i < mat.height; i++) {
            for (int j = 0; j < mat.width; j++) {
                s << std::setw(w);
                if (mat.offsets[i + 1] > cursor && mat.columnIndices[cursor] == j) {
                    s << mat.values[cursor];
                    cursor++;
                } else {
                    s << "0";
                }
            }

            if (i == mat.height - 1) {
                s << "]";
            }

            s << std::endl;
        }

        return s;
    }
}