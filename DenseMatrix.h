#pragma once

#include <vector>
#include <assert.h>
#include <iomanip>
#include "NumericTypeConcept.h"
#include "Triplet.h"

namespace Barta {

    template<NumericType T>
    class DenseMatrix {
        using Values = std::vector<std::vector<T>>;
        using TripletType = Triplet<T>;
        using VectorType = std::vector<T>;

        unsigned int width;
        unsigned int height;
        Values values;
    public:
        DenseMatrix(
            unsigned int width,
            unsigned int height
        ) :
            width(width),
            height(height)
        {
            this->values.resize(this->height);
            for (auto& row : this->values) {
                row.resize(this->width, static_cast<T>(0));
            }
        }
        DenseMatrix(
            unsigned int width,
            unsigned int height,
            std::vector<TripletType> triplets
        ) :
            DenseMatrix(
                width,
                height
            )
        {
            for (const auto triplet : triplets) {
                this->values[triplet.row][triplet.col] += triplet.val;
            }
        }

        VectorType operator * (const VectorType& v) const {
            assert(this->height == v.size());

            auto ret = VectorType(v.size(), static_cast<T>(0));
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    ret[i] += this->values[i][j] * v[j];
                }
            }

            return ret;
        }

        std::string toString() const {
            std::stringstream ss;
            constexpr const unsigned int w = 6;
            ss << "[";
            for (int i = 0; i < this->height; i++) {
                for (int j = 0; j < this->width; j++) {
                    ss << std::setw(w) << this->values[i][j];
                }

                if (i == this->height - 1) {
                    ss << "]";
                }

                ss << std::endl;
            }

            return ss.str();
        }

    };
}