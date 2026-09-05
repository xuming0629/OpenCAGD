#include <iostream>

#include <opencagd/curve/knot_vector.hpp>

int main()
{
    const opencagd::curve::KnotVector knots(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0});

    std::cout << "degree=" << knots.degree()
              << ", basis count=" << knots.num_basis_functions()
              << ", domain=[" << knots.domain_min() << ", " << knots.domain_max() << "]\n";

    for (double u : {0.0, 0.25, 0.999, 1.0, 1.5, 2.5, 3.0})
    {
        std::cout << "u=" << u << " -> span=" << knots.find_span(u) << '\n';
    }
}
