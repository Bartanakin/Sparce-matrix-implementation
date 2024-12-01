#include "../DenseMatrix.h"
#include "../SparseRowWiseMatrix.h"
#include <chrono>
#include <cmath>
#include <fstream>

int main() {
    unsigned int width;
    unsigned int height;
    unsigned int nnz;
    std::cin >> width >> height >> nnz;
    std::vector<Barta::Triplet<float>> triplets;
    triplets.reserve(nnz);

    unsigned int thread_count = 1;
    unsigned int initial_queue_space = std::sqrt(nnz);
    std::ifstream configFile("config.txt");
    std::string line;
    while (std::getline(configFile, line)) {
        if (line.substr(0, 11) == "THREAD_NUM=") {
            thread_count = std::stoi(line.substr(11));
        }

        if (line.substr(0, 21) == "INITIAL_QUEUE_SPACE=") {
            initial_queue_space = std::stoi(line.substr(21));
        }
    }

    for (unsigned int i = 0; i < nnz; i++) {
        unsigned int row, col;
        float value;
        std::cin >> row >> col >> value;
        triplets.emplace_back(row, col, value);
    }

    Barta::SparseRowWiseMatrix<float> A(width + 1, height + 1, std::move(triplets), false, true);

    auto beg = std::chrono::steady_clock::now();
    auto B1 = A.multiplyInner(A);
    std::cout << "Sequential inner product multiplication with NO transposition: ";
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - beg).count() << std::endl;

    beg = std::chrono::steady_clock::now();
    auto B2 = A.multiplyInnerWithTransposition(A, 2);
    std::cout << "Sequential inner product multiplication with transposition: ";
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - beg).count() << std::endl;

    beg = std::chrono::steady_clock::now();
    auto B3 = A.multiplyInnerWithTransposition(A, thread_count);
    std::cout << "Parallel inner product multiplication with transposition: ";
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - beg).count() << std::endl;

    beg = std::chrono::steady_clock::now();
    auto B4 = A.multiplyRowWise(A, initial_queue_space, 2);
    std::cout << "Sequential row wise product multiplication: ";
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - beg).count() << std::endl;

    beg = std::chrono::steady_clock::now();
    auto B5 = A.multiplyRowWise(A, initial_queue_space, thread_count);
    std::cout << "Parallel row wise product multiplication: ";
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - beg).count() << std::endl;

    return 0;
}
