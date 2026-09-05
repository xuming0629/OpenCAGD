#include <string>
#include <vector>

#include <opencagd/curve/bspline_basis.hpp>
#include <opencagd/visualization/matplotlib.hpp>

int main()
{
    const opencagd::curve::KnotVector knots(
        2,
        {0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 3.0, 3.0});

    constexpr std::size_t samples = 301;
    std::vector<double> u(samples);
    std::vector<std::vector<double>> curves(
        knots.num_basis_functions(), std::vector<double>(samples));
    std::vector<std::string> labels;
    for (std::size_t i = 0; i < knots.num_basis_functions(); ++i)
        labels.push_back("N_" + std::to_string(i) + ",2");

    for (std::size_t k = 0; k < samples; ++k)
    {
        u[k] = knots.domain_min() +
               (knots.domain_max() - knots.domain_min()) *
               static_cast<double>(k) / static_cast<double>(samples - 1);
        const auto values = opencagd::curve::all_basis_functions(knots, u[k]);
        for (std::size_t i = 0; i < values.size(); ++i) curves[i][k] = values[i];
    }

    opencagd::visualization::PlotOptions options;
    options.title = "Quadratic B-spline basis";
    options.xlabel = "u";
    options.ylabel = "N_i,2(u)";
    options.output_path = "outputs/bspline_basis.png";
    opencagd::visualization::plot_basis_functions(u, curves, labels, options);
}
