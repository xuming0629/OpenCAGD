#include <opencagd/linalg/dense_matrix.h>

#include <stdexcept>

namespace opencagd {
namespace linalg {

DenseMatrix::DenseMatrix() : rows_(0), cols_(0), values_() {}

DenseMatrix::DenseMatrix(std::size_t rows, std::size_t cols, double value)
    : rows_(rows), cols_(cols), values_(rows * cols, value)
{
}

std::size_t DenseMatrix::rows() const { return rows_; }
std::size_t DenseMatrix::cols() const { return cols_; }

static std::size_t checked_index(std::size_t row, std::size_t col,
                                 std::size_t rows, std::size_t cols)
{
    if (row >= rows || col >= cols)
        throw std::out_of_range("DenseMatrix index out of range");
    return row * cols + col;
}

double& DenseMatrix::operator()(std::size_t row, std::size_t col)
{
    return values_[checked_index(row, col, rows_, cols_)];
}

double DenseMatrix::operator()(std::size_t row, std::size_t col) const
{
    return values_[checked_index(row, col, rows_, cols_)];
}

const std::vector<double>& DenseMatrix::values() const { return values_; }

} // namespace linalg
} // namespace opencagd
