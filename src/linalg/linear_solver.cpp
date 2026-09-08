#include <opencagd/linalg/linear_solver.h>

#include <stdexcept>
#include <Eigen/Dense>

namespace opencagd {
namespace linalg {
namespace {
Eigen::MatrixXd to_eigen(const DenseMatrix& matrix)
{
    Eigen::MatrixXd result(static_cast<Eigen::Index>(matrix.rows()),
                           static_cast<Eigen::Index>(matrix.cols()));
    for (std::size_t i = 0; i < matrix.rows(); ++i)
        for (std::size_t j = 0; j < matrix.cols(); ++j)
            result(static_cast<Eigen::Index>(i), static_cast<Eigen::Index>(j)) = matrix(i, j);
    return result;
}

Eigen::VectorXd to_eigen(const std::vector<double>& values)
{
    Eigen::VectorXd result(static_cast<Eigen::Index>(values.size()));
    for (std::size_t i = 0; i < values.size(); ++i)
        result(static_cast<Eigen::Index>(i)) = values[i];
    return result;
}

std::vector<double> from_eigen(const Eigen::VectorXd& values)
{
    std::vector<double> result(static_cast<std::size_t>(values.size()));
    for (Eigen::Index i = 0; i < values.size(); ++i)
        result[static_cast<std::size_t>(i)] = values(i);
    return result;
}
}

std::vector<double> solve_linear_system(const DenseMatrix& matrix,
                                        const std::vector<double>& rhs)
{
    if (matrix.rows() != matrix.cols())
        throw std::invalid_argument("linear system matrix must be square");
    if (matrix.rows() != rhs.size())
        throw std::invalid_argument("linear system rhs size mismatch");
    if (matrix.rows() == 0)
        return std::vector<double>();

    const Eigen::MatrixXd a = to_eigen(matrix);
    const Eigen::VectorXd b = to_eigen(rhs);
    const Eigen::FullPivLU<Eigen::MatrixXd> lu(a);
    if (!lu.isInvertible())
        throw std::runtime_error("linear system is singular");
    return from_eigen(lu.solve(b));
}

std::vector<double> solve_least_squares(const DenseMatrix& matrix,
                                        const std::vector<double>& rhs)
{
    if (matrix.rows() != rhs.size())
        throw std::invalid_argument("least-squares rhs size mismatch");
    if (matrix.rows() == 0 || matrix.cols() == 0)
        return std::vector<double>();

    const Eigen::MatrixXd a = to_eigen(matrix);
    const Eigen::VectorXd b = to_eigen(rhs);
    return from_eigen(a.colPivHouseholderQr().solve(b));
}

} // namespace linalg
} // namespace opencagd
