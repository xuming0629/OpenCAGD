#include <iomanip>
#include <iostream>

#include <opencagd/math/power_basis.hpp>

int main()
{
    const double t = 0.25;
    const auto values = opencagd::math::power_basis(4, t);

    std::cout << "Power basis at t=" << t << "\n";
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        std::cout << "t^" << i << " = " << std::setprecision(10) << values[i] << '\n';
    }
}
