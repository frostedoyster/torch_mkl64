#include <torch/extension.h>
#include <iostream>
#include <stdexcept>
#include "mkl.h"


namespace mkl64 {


    std::vector<torch::Tensor> dsyev(torch::Tensor A) {

        if (A.sizes().size() != 2) throw std::runtime_error("not two-dimensional");
        if (A.sizes()[0] != A.sizes()[1]) throw std::runtime_error("not square");
        if (A.dtype() != torch::kFloat64) throw std::runtime_error("not float64");
        if (!A.is_contiguous()) throw std::runtime_error("not contiguous");

        long long n = A.sizes()[0];
        torch::Tensor O = A.detach().clone();
        torch::Tensor d = torch::empty({A.sizes()[0]}, torch::TensorOptions().dtype(torch::kFloat64));

        int info = LAPACKE_dsyev(LAPACK_ROW_MAJOR, 'V', 'U', n, O.data_ptr<double>(), n, d.data_ptr<double>());
        if (info != 0) std::cout << "Something went wrong: info code " << info << std::endl;

        return std::vector<torch::Tensor>{d, O};
    }


    torch::Tensor dgesv(torch::Tensor A, torch::Tensor b) {

        if (A.sizes().size() != 2) throw std::runtime_error("not two-dimensional");
        if (A.sizes()[0] != A.sizes()[1]) throw std::runtime_error("not square");
        if (A.dtype() != torch::kFloat64) throw std::runtime_error("not float64");
        if (!A.is_contiguous()) throw std::runtime_error("not contiguous");
        if (!b.is_contiguous()) throw std::runtime_error("not contiguous");
        if (A.sizes()[0] != b.sizes()[0]) throw std::runtime_error("size inconsistency");

        long long n = A.sizes()[0];
        torch::Tensor A_copy = A.detach().clone();
        torch::Tensor x = b.detach().clone();
        std::vector<long long> ipiv(n);

        int info = LAPACKE_dgesv(LAPACK_ROW_MAJOR, n, 1, A_copy.data_ptr<double>(), n, ipiv.data(), x.data_ptr<double>(), 1);
        if (info != 0) std::cout << "Something went wrong: info code " << info << std::endl;

        return x;
    }


    torch::Tensor dsysv(torch::Tensor A, torch::Tensor b) {

        if (A.sizes().size() != 2) throw std::runtime_error("not two-dimensional");
        if (A.sizes()[0] != A.sizes()[1]) throw std::runtime_error("not square");
        if (A.dtype() != torch::kFloat64) throw std::runtime_error("not float64");
        if (!A.is_contiguous()) throw std::runtime_error("not contiguous");
        if (!b.is_contiguous()) throw std::runtime_error("not contiguous");
        if (A.sizes()[0] != b.sizes()[0]) throw std::runtime_error("size inconsistency");

        long long n = A.sizes()[0];
        torch::Tensor A_copy = A.detach().clone();
        torch::Tensor x = b.detach().clone();
        std::vector<long long> ipiv(n);

        int info = LAPACKE_dsysv(LAPACK_ROW_MAJOR, 'U', n, 1, A_copy.data_ptr<double>(), n, ipiv.data(), x.data_ptr<double>(), 1);
        if (info != 0) std::cout << "Something went wrong: info code " << info << std::endl;

        return x;
    }


}


PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
    m.def("dsyev", &mkl64::dsyev, "dsyev: diagonalization (symmetric)");
    m.def("dgesv", &mkl64::dgesv, "dgesv: linear solve (general)");
    m.def("dsysv", &mkl64::dsysv, "dsysv: linear solve (symmetric)");
}
