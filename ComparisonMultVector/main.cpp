#include <chrono>
#include "../SparseRowWiseMatrix.h"
#include "../DenseMatrix.h"

int main() {
    std::vector<Barta::Triplet<int>> triplets;
    std::srand(std::time(nullptr));

    constexpr const unsigned int nnzFactor = 3;

    for (int i = 0; i < 100; i++) {
        unsigned int matrixSize = 50 * i;

        std::cout << matrixSize << ";";
        triplets.reserve(matrixSize * nnzFactor);

        for (int i = 0; i < matrixSize; i++) {
            triplets.emplace_back(i, i, std::rand() % 100);
        }

        for (int i = 0; i < matrixSize * (nnzFactor - 1); i++) {
            triplets.emplace_back(std::rand() % matrixSize, std::rand() % matrixSize, std::rand() % 100);
        }

        std::vector<int> w;
        w.reserve(matrixSize);
        for (int i = 0; i < matrixSize; i++) {
            w.push_back(std::rand() % 100);
        }

        auto denseA = Barta::DenseMatrix<int>(
            matrixSize,
            matrixSize,
            triplets
        );

        auto sparseA = Barta::SparseRowWiseMatrix<int>(
            matrixSize,
            matrixSize,
            triplets
        );

        std::chrono::steady_clock clock;
        auto beg = std::chrono::steady_clock::now();
        auto denseV = denseA*w;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - beg).count()  << ";";

        beg = std::chrono::steady_clock::now();
        auto sparseV = sparseA*w;
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - beg).count();

        for (int i = 0; i < matrixSize; i++) {
            assert(denseV[i] == sparseV[i]);
        }

        std::cout << std::endl;
    }

    return 0;
}
