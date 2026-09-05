#include <opencagd/visualization/matplotlib.hpp>

#include <Python.h>

#include <iomanip>
#include <mutex>
#include <sstream>
#include <stdexcept>

namespace opencagd::visualization
{
namespace
{

std::mutex python_mutex;

std::string py_string(const std::string& input)
{
    std::string result = "'";
    for (const char ch : input)
    {
        if (ch == '\\' || ch == '\'') result.push_back('\\');
        if (ch == '\n') result += "\\n";
        else result.push_back(ch);
    }
    result.push_back('\'');
    return result;
}

std::string doubles_to_python(const std::vector<double>& values)
{
    std::ostringstream stream;
    stream << std::setprecision(17) << '[';
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        if (i != 0) stream << ',';
        stream << values[i];
    }
    stream << ']';
    return stream.str();
}

std::string coordinates_to_python(const std::vector<Point2d>& points, std::size_t axis)
{
    std::vector<double> values;
    values.reserve(points.size());
    for (const auto& point : points) values.push_back(point[axis]);
    return doubles_to_python(values);
}

void execute_python(const std::string& script)
{
    std::lock_guard lock(python_mutex);
    if (!Py_IsInitialized()) Py_Initialize();

    const int rc = PyRun_SimpleString(script.c_str());
    if (rc != 0)
    {
        if (PyErr_Occurred()) PyErr_Print();
        throw std::runtime_error("Embedded Python/Matplotlib execution failed");
    }
}

void append_common_plot_options(std::ostringstream& py, const PlotOptions& options)
{
    if (!options.title.empty()) py << "plt.title(" << py_string(options.title) << ")\n";
    if (!options.xlabel.empty()) py << "plt.xlabel(" << py_string(options.xlabel) << ")\n";
    if (!options.ylabel.empty()) py << "plt.ylabel(" << py_string(options.ylabel) << ")\n";
    if (options.grid) py << "plt.grid(True, alpha=0.3)\n";
    if (options.equal_axis) py << "plt.axis('equal')\n";
    py << "plt.tight_layout()\n";
    if (!options.output_path.empty())
    {
        py << "plt.savefig(" << py_string(options.output_path) << ", dpi=180, bbox_inches='tight')\n";
    }
    if (options.show) py << "plt.show()\n";
    else py << "plt.close()\n";
}

} // namespace

void plot_curve_2d(
    const std::vector<Point2d>& curve_points,
    const std::vector<Point2d>& control_points,
    const PlotOptions& options)
{
    if (curve_points.empty()) throw std::invalid_argument("curve_points must not be empty");

    std::ostringstream py;
    if (!options.show) py << "import matplotlib\nmatplotlib.use('Agg')\n";
    py << "import matplotlib.pyplot as plt\n";
    py << "cx=" << coordinates_to_python(curve_points, 0) << "\n";
    py << "cy=" << coordinates_to_python(curve_points, 1) << "\n";
    py << "plt.figure(figsize=(7.2,5.4))\n";
    py << "plt.plot(cx, cy, linewidth=2.0, label='curve')\n";

    if (!control_points.empty())
    {
        py << "px=" << coordinates_to_python(control_points, 0) << "\n";
        py << "py=" << coordinates_to_python(control_points, 1) << "\n";
        py << "plt.plot(px, py, '--o', linewidth=1.1, markersize=5, label='control polygon')\n";
    }
    py << "plt.legend()\n";
    append_common_plot_options(py, options);
    execute_python(py.str());
}

void plot_basis_functions(
    const std::vector<double>& parameters,
    const std::vector<std::vector<double>>& basis_values,
    const std::vector<std::string>& labels,
    const PlotOptions& options)
{
    if (parameters.empty() || basis_values.empty())
    {
        throw std::invalid_argument("basis plot data must not be empty");
    }
    if (basis_values.size() != labels.size())
    {
        throw std::invalid_argument("basis_values and labels must have the same size");
    }
    for (const auto& values : basis_values)
    {
        if (values.size() != parameters.size())
        {
            throw std::invalid_argument("every basis curve must match parameter count");
        }
    }

    std::ostringstream py;
    if (!options.show) py << "import matplotlib\nmatplotlib.use('Agg')\n";
    py << "import matplotlib.pyplot as plt\n";
    py << "u=" << doubles_to_python(parameters) << "\n";
    py << "plt.figure(figsize=(7.2,5.4))\n";
    for (std::size_t i = 0; i < basis_values.size(); ++i)
    {
        py << "v=" << doubles_to_python(basis_values[i]) << "\n";
        py << "plt.plot(u, v, linewidth=1.8, label=" << py_string(labels[i]) << ")\n";
    }
    py << "plt.legend()\n";
    append_common_plot_options(py, options);
    execute_python(py.str());
}

} // namespace opencagd::visualization
