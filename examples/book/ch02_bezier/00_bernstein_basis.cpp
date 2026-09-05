#include <iomanip>
#include <iostream>
#include <numeric>

#include <opencagd/curve/bernstein.hpp>

int main()
{
    constexpr std::size_t degree = 3;

    for (double t : {0.0, 0.25, 0.5, 0.75, 1.0})
    {
        const auto basis = opencagd::curve::bernstein_basis(degree, t);
        const double sum = std::accumulate(basis.begin(), basis.end(), 0.0);

        std::cout << "t=" << t << " : ";
        for (double value : basis) std::cout << std::setw(10) << value << ' ';
        std::cout << "  sum=" << sum << '\n';
    }
}
