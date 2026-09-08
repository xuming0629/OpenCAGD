#include <cstddef>
#include <sstream>
#include <string>
#include <vector>
#include <opencagd/curve.h>
#include "example_backend.h"

int main()
{
    const std::size_t degree = 3;
    const std::vector<double> knots{0,0,0,0,0.25,0.50,0.75,1,1,1,1};
    const opencagd::curve::KnotVector kv(degree, knots);
    const std::size_t basis_count = kv.num_basis_functions();
    const std::size_t sample_count = 401;

    opencagd::visualization::BasisFunctionData data;
    data.parameters.resize(sample_count, 0.0);
    data.values.assign(basis_count, std::vector<double>(sample_count, 0.0));
    const opencagd::geometry::Interval domain = kv.domain();
    for (std::size_t k = 0; k < sample_count; ++k)
    {
        const double a = static_cast<double>(k) / static_cast<double>(sample_count - 1);
        const double u = domain.min + domain.length() * a;
        data.parameters[k] = u;
        const std::vector<double> basis = opencagd::curve::all_basis_functions(kv, u);
        for (std::size_t i = 0; i < basis_count; ++i) data.values[i][k] = basis[i];
    }
    for (std::size_t i = 0; i < basis_count; ++i)
    {
        std::ostringstream label; label << "N_" << i << ",3"; data.labels.push_back(label.str());
    }

    opencagd::visualization::RenderOptions options;
    options.title = "Cubic B-Spline Basis Functions";
    options.xlabel = "u"; options.ylabel = "N_i,3(u)"; options.equal_axis = false;
    opencagd_example::finalize_options(options, "04_bspline_basis.png");

    opencagd_example::Renderer renderer;
    renderer.render(data, options);
    return 0;
}
