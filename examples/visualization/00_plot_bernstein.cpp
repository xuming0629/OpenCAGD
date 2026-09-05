#include <string>
#include <vector>

#include <opencagd/curve/bernstein.hpp>
#include <opencagd/visualization/matplotlib.hpp>

int main()
{
    constexpr std::size_t degree = 4;
    constexpr std::size_t samples = 201;
    std::vector<double> t(samples);
    std::vector<std::vector<double>> curves(degree + 1, std::vector<double>(samples));
    std::vector<std::string> labels;

    for (std::size_t i = 0; i <= degree; ++i) labels.push_back("B_" + std::to_string(i) + "^4");
    for (std::size_t k = 0; k < samples; ++k)
    {
        t[k] = static_cast<double>(k) / static_cast<double>(samples - 1);
        const auto values = opencagd::curve::bernstein_basis(degree, t[k]);
        for (std::size_t i = 0; i <= degree; ++i) curves[i][k] = values[i];
    }

    opencagd::visualization::PlotOptions options;
    options.title = "Bernstein basis, degree 4";
    options.xlabel = "t";
    options.ylabel = "B_i^4(t)";
    options.output_path = "outputs/bernstein_basis.png";
    opencagd::visualization::plot_basis_functions(t, curves, labels, options);
}
