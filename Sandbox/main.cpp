

#include "../SparseRowWiseMatrix.h"
#include "../DenseMatrix.h"

int main() {
    std::vector<Barta::Triplet<float>> triplets =
        {
            {0, 0, 1.f}, {0, 1, 7.f}, {0, 4, 3.f},
            {1, 1, 1.f}, {1, 1, 1.f},
            {2, 1, 3.f}, {2, 2, 6.f}, {2, 4, 1.f},
            {3, 1, 5.f}, {3, 3, 4.f}, {3, 4, 10.f},
            {4, 2, 1.f}
        };
    Barta::SparseRowWiseMatrix<float> A(
        5,
        5,
        triplets
    );
    A.insert(4, 4, 8.f);
    Barta::SparseRowWiseMatrix<float>::VectorType v = {1.f, -1.f, 0.f, 3.f, 1.f};

    Barta::DenseMatrix<float> B(
        5,
        5,
        triplets
    );

    std::cout << "A: " << A.toString() << std::endl;
    std::cout << "B: " << B.toString() << std::endl;

    auto w = A*v;

    for (const auto x : w) {
        std::cout << x << " ";
    }

    std::cout << std::endl;

    auto w2 = B*v;

    for (const auto x : w2) {
        std::cout << x << " ";
    }

    std::cout << std::endl;

    return 0;
}
