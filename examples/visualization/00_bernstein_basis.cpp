#include <cstddef>
#include <sstream>
#include <string>
#include <vector>
#include <opencagd/curve.h>
#include "example_backend.h"

int main()
{
    const std::size_t degree = 4;
    const std::size_t sample_count = 201;

    opencagd::visualization::BasisFunctionData data;
    data.parameters.resize(sample_count, 0.0);
    data.values.assign(degree + 1, std::vector<double>(sample_count, 0.0));

    for (std::size_t k = 0; k < sample_count; ++k)
    {
        const double t = static_cast<double>(k) / static_cast<double>(sample_count - 1);
        data.parameters[k] = t;
        const std::vector<double> basis = opencagd::curve::bernstein_basis(degree, t);
        for (std::size_t i = 0; i <= degree; ++i) data.values[i][k] = basis[i];
    }

    for (std::size_t i = 0; i <= degree; ++i)
    {
        std::ostringstream label;
        label << "B_" << i << "^" << degree;
        data.labels.push_back(label.str());
    }

    opencagd::visualization::RenderOptions options;
    options.title = "Bernstein Basis Functions (degree 4)";
    options.xlabel = "t";
    options.ylabel = "B_i^4(t)";
    options.equal_axis = false;
    opencagd_example::finalize_options(options, "00_bernstein_basis.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}
