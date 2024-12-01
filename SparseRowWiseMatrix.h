#pragma once

#include "NumericTypeConcept.h"
#include "RowQueue.h"
#include "Triplet.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>

namespace Barta {

template<NumericType T>
class SparseRowWiseMatrix {
    public:
    unsigned int width;
    unsigned int height;

    std::vector<T> values;
    std::vector<unsigned int> columnIndices;
    std::vector<unsigned int> offsets;

public:
    using VectorType = std::vector<T>;
    using TripletType = Triplet<T>;

    SparseRowWiseMatrix(
        unsigned int width,
        unsigned int height
    ):
        width(width),
        height(height),
        offsets(height + 1, 0) {}

    SparseRowWiseMatrix(
        unsigned int width,
        unsigned int height,
        std::vector<TripletType> triplets,
        bool sorted = false,
        bool merged = false
    ):
        SparseRowWiseMatrix(width, height) {
        if (!sorted) {
            TripletType::sort(triplets);
        }

        std::vector<TripletType> mergedTriplets;
        if (!merged) {
            mergedTriplets = TripletType::mergeSorted(triplets);
        } else {
            mergedTriplets = std::move(triplets);
        }

        this->values.resize(mergedTriplets.size(), static_cast<T>(0));
        this->columnIndices.resize(mergedTriplets.size(), static_cast<T>(0));
        auto rowSizes = std::vector<std::size_t>(this->height);
        unsigned int i = 0;
        for (const auto triplet: mergedTriplets) {
            this->values[i] = triplet.val;
            this->columnIndices[i] = triplet.col;
            ++this->offsets[triplet.row];

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

    SparseRowWiseMatrix(
        unsigned int width,
        unsigned int height,
        std::vector<std::vector<TripletType>> vectorOfTriplets // assumption: sorted and merged
    ):
        SparseRowWiseMatrix(width, height) {
        size_t nnz = 0;
        for (const auto& triplets: vectorOfTriplets) {
            nnz += triplets.size();
        }

        this->values.resize(nnz, static_cast<T>(0));
        this->columnIndices.resize(nnz, static_cast<T>(0));
        auto rowSizes = std::vector<std::size_t>(this->height);
        unsigned int i = 0;
        for (const auto& triplets: vectorOfTriplets) {
            for (const auto triplet: triplets) {
                this->values[i] = triplet.val;
                this->columnIndices[i] = triplet.col;
                ++this->offsets[triplet.row];

                i++;
            }
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

    void insert(
        unsigned int row,
        unsigned int col,
        T value
    ) {
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

    VectorType operator*(
        const VectorType& v
    ) const {
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
        constexpr unsigned int w = 6;
        ss << "[" << std::endl;
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
                ss << std::endl << "]";
            }

            ss << std::endl;
        }

        return ss.str();
    }

    SparseRowWiseMatrix multiplyInner(
        const SparseRowWiseMatrix& other
    ) const {
        std::vector<TripletType> triplets = {};
        triplets.reserve(std::max(this->values.size(), other.values.size()));
        for (int row_l = 0; row_l < this->offsets.size() - 1; row_l++) {
            // if (row_l % (this->height / 100) == 0) {
            //     std::cout << "calculating row " << row_l << std::endl;
            // }

            for (int col_r = 0; col_r < other.width; col_r++) {
                T value = static_cast<T>(0);
                for (int i_l = this->offsets[row_l]; i_l < this->offsets[row_l + 1]; ++i_l) {
                    auto col_l = this->columnIndices[i_l];
                    auto row_r = col_l;
                    for (int i_r = other.offsets[row_r]; i_r < other.offsets[row_r + 1]; ++i_r) {
                        if (other.columnIndices[i_r] == col_r) {
                            value += this->values[i_l] * other.values[i_r];

                            break;
                        }
                    }
                }

                if (value != static_cast<T>(0)) {
                    triplets.emplace_back(row_l, col_r, value);
                }
            }
        }

        return SparseRowWiseMatrix(other.width, this->height, std::move(triplets), true, true);
    }


    SparseRowWiseMatrix multiplyInnerWithTransposition(
        const SparseRowWiseMatrix& other,
        unsigned int thread_num
    ) const {
        std::vector<TripletType> triplets = {};
        triplets.reserve(other.values.size());
        int row = 0;
        for (int i = 0; i < other.values.size(); i++) {
            while (other.offsets[row + 1] <= i) {
                ++row;
            }

            triplets.emplace_back(other.columnIndices[i], row, other.values[i]);
        }

        SparseRowWiseMatrix transposedOther(other.height, other.width, std::move(triplets), false, true);

        std::vector<std::vector<TripletType>> vectorOfTriplets(this->offsets.size() - 1);

        std::vector<std::thread> threads;
        threads.reserve(thread_num + 1);
        std::atomic<int> counter(0);
        for (int i = 0; i < thread_num; i++) {
            threads.emplace_back(
                [this, &transposedOther, &counter, &vectorOfTriplets] () {
                    while (true) {
                        int row_l = counter++;
                        if (row_l > this->height) {
                            break;
                        }

                        // if (row_l % (this->height / 100) == 0) {
                        //     std::cout << "calculating row " << row_l << std::endl;
                        // }

                        vectorOfTriplets[row_l].reserve(std::sqrt(this->height)); // TODO dynamic memory size
                        for (int row_r = 0; row_r < transposedOther.height; row_r++) {
                            auto i_l = this->offsets[row_l];
                            auto i_r = transposedOther.offsets[row_r];
                            T value = static_cast<T>(0);
                            while (i_l < this->offsets[row_l + 1] && i_r < transposedOther.offsets[row_r + 1]) {
                                if (this->columnIndices[i_l] < transposedOther.columnIndices[i_r]) {
                                    ++i_l;
                                } else if (this->columnIndices[i_l] > transposedOther.columnIndices[i_r]) {
                                    ++i_r;
                                } else {
                                    value += this->values[i_l] * transposedOther.values[i_r];
                                    ++i_l;
                                    ++i_r;
                                }
                            }

                            if (value != static_cast<T>(0)) {
                                vectorOfTriplets[row_l].emplace_back(row_l, row_r, value);
                            }
                        }
                    }
                }
            );
        }

        for (int i = 0; i < thread_num; i++) {
            threads[i].join();
        }

        return SparseRowWiseMatrix(other.width, this->height, std::move(vectorOfTriplets));
    }

    SparseRowWiseMatrix multiplyRowWise(
        const SparseRowWiseMatrix& other,
        const unsigned int initialQueueCapacity,
        const unsigned int thread_num
    ) const {
        std::vector<std::vector<TripletType>> vectorOfTriplets(this->offsets.size() - 1);

        std::vector<std::thread> threads;
        threads.reserve(thread_num + 1);
        std::atomic<int> counter(0);
        for (int i = 0; i < thread_num; i++) {
            threads.emplace_back(
                [this, &other, initialQueueCapacity, &counter, &vectorOfTriplets] () {
                    while (true) {
                        int row_l = counter++;
                        if (row_l > this->height) {
                            break;
                        }

                        // if (row_l % (this->height / 100) == 0) {
                        //     std::cout << "calculating row " << row_l << std::endl;
                        // }

                        RowQueue<T> queue = {initialQueueCapacity};
                        for (int i_l = this->offsets[row_l]; i_l < this->offsets[row_l + 1]; ++i_l) {
                            auto row_r = this->columnIndices[i_l];
                            auto value_l = this->values[i_l];
                            for (int i_r = other.offsets[row_r]; i_r < other.offsets[row_r + 1]; ++i_r) {
                                queue.push(value_l * other.values[i_r], other.columnIndices[i_r]);
                            }
                        }

                        if (queue.size() < 1) {
                            return;
                        }

                        queue.mergeAll();
                        auto& elements = queue.getElements();
                        vectorOfTriplets[row_l].resize(elements.size());
                        int i = queue.getQueueBeg()[0];
                        int j = 0;
                        while (i != -1) {
                            vectorOfTriplets[row_l][j] = {row_l, elements[i].col, elements[i].value};

                            ++j;
                            i = elements[i].next;
                        }
                    }
                }
            );
        }

        for (int i = 0; i < thread_num; i++) {
            threads[i].join();
        }

        return SparseRowWiseMatrix(other.width, this->height, vectorOfTriplets);
    }

    bool operator==(const SparseRowWiseMatrix & other) const {
        if (this->values.size() != other.values.size()) {
            return false;
        }

        if (this->columnIndices.size() != other.columnIndices.size()) {
            return false;
        }

        if (this->offsets.size() != other.offsets.size()) {
            return false;
        }

        for (int i = 0; i < this->values.size(); ++i) {
            auto err = (this->values[i] - other.values[i]) / (this->values[i] + other.values[i]);
            if (std::abs(err) > 1e-3) {
                std::cout << "XD4 " << i << std::endl;
                return false;
            }
        }

        for (int i = 0; i < this->columnIndices.size(); ++i) {
            if (this->columnIndices[i] != other.columnIndices[i]) {
            std::cout << "XD5 " << i << std::endl;
                return false;
            }
        }

        for (int i = 0; i < this->offsets.size(); ++i) {
            if (this->offsets[i] != other.offsets[i]) {
            std::cout << "XD6 " << i << std::endl;
                return false;
            }
        }

        return true;
    }
};

template<NumericType T>
std::ostream& operator<<(
    std::ostream& s,
    const SparseRowWiseMatrix<T>& mat
) {
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
