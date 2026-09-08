#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>

namespace opencagd {
namespace linalg {

class OPENCAGD_LINALG_API DenseMatrix
{
public:
    DenseMatrix();
    DenseMatrix(std::size_t rows, std::size_t cols, double value = 0.0);

    std::size_t rows() const;
    std::size_t cols() const;
    double& operator()(std::size_t row, std::size_t col);
    double operator()(std::size_t row, std::size_t col) const;
    const std::vector<double>& values() const;

private:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<double> values_;
};

} // namespace linalg
} // namespace opencagd
