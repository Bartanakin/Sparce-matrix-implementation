#include "../DenseMatrix.h"
#include "../SparseRowWiseMatrix.h"

int main() {
    std::vector<Barta::Triplet<float>> tripletsA = {
        {0, 0, 1.f },
        {0, 1, 7.f },
        {0, 4, 3.f },
        {1, 1, 1.f },
        {1, 1, 1.f },
        {2, 1, 3.f },
        {2, 2, 6.f },
        {2, 4, 1.f },
        {3, 1, 5.f },
        {3, 3, 4.f },
        {3, 4, 10.f},
        {4, 2, 1.f },
        {4, 4, 8.f },
        {1, 5, 1.f },
        {1, 6, 2.f },
        {4, 6, -5.f },
        {6, 6, -7.f },
        {6, 1, -2.f },
        {5, 2, 3.f },
        {6, 4, 6.f },
    };
    Barta::SparseRowWiseMatrix A(7, 7, tripletsA);

    std::vector<Barta::Triplet<float>> tripletsB = {
        {0, 1, 2.f },
        {0, 2, 4.f },
        {1, 0, 1.f },
        {1, 1, 1.f },
        {2, 0, 3.f },
        {2, 4, 5.f },
        {3, 0, -2.f },
        {3, 2, -1.f },
        {4, 2, 3.f },
        {4, 3, 7.f },
        {4, 5, -3.f },
        {5, 0, 5.f },
        {6, 1, 3.f },
        {5, 4, 1.f },
        {1, 6, -2.f },
        {6, 6, 11.f },
    };
    Barta::SparseRowWiseMatrix B(7, 7, tripletsB);

    std::cout << "A: " << A.toString() << std::endl;
    std::cout << "B: " << B.toString() << std::endl;

    auto C = A.multiplyInner(B);

    std::cout << "C: " << C.toString() << std::endl;

    auto C2 = A.multiplyInnerWithTransposition(B, 2);

    std::cout << "C2: " << C2.toString() << std::endl;

    auto D = A.multiplyRowWise(B, 4, 2);

    std::cout << "D: " << D.toString() << std::endl;

    assert(C == D);

    std::cout << std::endl;

    return 0;
}
