#include <opencagd/visualization/matplotlib.hpp>
#include <opencagd/visualization/python_runtime.hpp>

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace opencagd::visualization
{
namespace
{
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
    for (const auto& p : points) values.push_back(p[axis]);
    return doubles_to_python(values);
}

std::string coordinates3_to_python(const std::vector<Point3d>& points, std::size_t axis)
{
    std::vector<double> values;
    values.reserve(points.size());
    for (const auto& p : points) values.push_back(p[axis]);
    return doubles_to_python(values);
}

void append_common(std::ostringstream& py, const PlotOptions& o, bool three_d)
{
    if (!o.title.empty())
        py << (three_d ? "ax.set_title(" : "plt.title(") << py_string(o.title) << ")\n";
    if (!o.xlabel.empty())
        py << (three_d ? "ax.set_xlabel(" : "plt.xlabel(") << py_string(o.xlabel) << ")\n";
    if (!o.ylabel.empty())
        py << (three_d ? "ax.set_ylabel(" : "plt.ylabel(") << py_string(o.ylabel) << ")\n";
    if (three_d && !o.zlabel.empty()) py << "ax.set_zlabel(" << py_string(o.zlabel) << ")\n";
    if (o.grid) py << (three_d ? "ax.grid(True, alpha=0.3)\n" : "plt.grid(True, alpha=0.3)\n");
    if (!three_d && o.equal_axis) py << "plt.axis('equal')\n";
    if (three_d && o.equal_axis) py << "ax.set_box_aspect((1,1,1))\n";
    py << "plt.tight_layout()\n";
    if (!o.output_path.empty())
        py << "plt.savefig(" << py_string(o.output_path) << ", dpi=180, bbox_inches='tight')\n";
    if (o.show) py << "plt.show()\n";
    else py << "plt.close()\n";
}

void append_backend(std::ostringstream& py, const PlotOptions& o)
{
    if (!o.show) py << "import matplotlib\nmatplotlib.use('Agg')\n";
    py << "import matplotlib.pyplot as plt\n";
}
} // namespace

void plot_curve_2d(const std::vector<Point2d>& curve_points,
                   const std::vector<Point2d>& control_points,
                   const PlotOptions& options)
{
    if (curve_points.empty()) throw std::invalid_argument("curve_points must not be empty");
    std::ostringstream py;
    append_backend(py, options);
    py << "cx=" << coordinates_to_python(curve_points, 0) << "\n";
    py << "cy=" << coordinates_to_python(curve_points, 1) << "\n";
    py << "plt.figure(figsize=(7.2,5.4))\n";
    py << "plt.plot(cx,cy,linewidth=2.0,label='curve')\n";
    if (!control_points.empty())
    {
        py << "px=" << coordinates_to_python(control_points, 0) << "\n";
        py << "py=" << coordinates_to_python(control_points, 1) << "\n";
        py << "plt.plot(px,py,'--o',linewidth=1.1,markersize=5,label='control polygon')\n";
    }
    py << "plt.legend()\n";
    append_common(py, options, false);
    PythonRuntime::instance().execute(py.str());
}

void plot_basis_functions(const std::vector<double>& parameters,
                          const std::vector<std::vector<double>>& basis_values,
                          const std::vector<std::string>& labels,
                          const PlotOptions& options)
{
    if (parameters.empty() || basis_values.empty())
        throw std::invalid_argument("basis plot data must not be empty");
    if (basis_values.size() != labels.size())
        throw std::invalid_argument("basis_values and labels must have the same size");
    for (const auto& values : basis_values)
        if (values.size() != parameters.size())
            throw std::invalid_argument("every basis curve must match parameter count");

    std::ostringstream py;
    append_backend(py, options);
    py << "u=" << doubles_to_python(parameters) << "\n";
    py << "plt.figure(figsize=(7.2,5.4))\n";
    for (std::size_t i = 0; i < basis_values.size(); ++i)
    {
        py << "v=" << doubles_to_python(basis_values[i]) << "\n";
        py << "plt.plot(u,v,linewidth=1.8,label=" << py_string(labels[i]) << ")\n";
    }
    py << "plt.legend()\n";
    append_common(py, options, false);
    PythonRuntime::instance().execute(py.str());
}

void plot_surface_3d(const surface::SurfaceSampleGrid<Point3d>& grid,
                     const std::vector<std::vector<Point3d>>& control_net,
                     const PlotOptions& options)
{
    if (grid.empty() || grid.u_count < 2 || grid.v_count < 2)
        throw std::invalid_argument("surface grid must be at least 2 x 2");

    std::vector<double> x, y, z;
    x.reserve(grid.points.size()); y.reserve(grid.points.size()); z.reserve(grid.points.size());
    for (const auto& p : grid.points) { x.push_back(p[0]); y.push_back(p[1]); z.push_back(p[2]); }

    std::ostringstream py;
    append_backend(py, options);
    py << "import numpy as np\n";
    py << "x=np.array(" << doubles_to_python(x) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    py << "y=np.array(" << doubles_to_python(y) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    py << "z=np.array(" << doubles_to_python(z) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    py << "fig=plt.figure(figsize=(8.2,6.4))\nax=fig.add_subplot(111,projection='3d')\n";
    py << "ax.plot_surface(x,y,z,alpha=0.82,linewidth=0,antialiased=True)\n";
    if (!control_net.empty())
    {
        const std::size_t rows = control_net.size();
        const std::size_t cols = control_net.front().size();
        for (const auto& row : control_net)
            if (row.size() != cols) throw std::invalid_argument("control net must be rectangular");
        for (std::size_t i = 0; i < rows; ++i)
        {
            py << "ax.plot(" << coordinates3_to_python(control_net[i],0) << ','
               << coordinates3_to_python(control_net[i],1) << ','
               << coordinates3_to_python(control_net[i],2)
               << ",'--o',linewidth=1.0,markersize=3)\n";
        }
        for (std::size_t j = 0; j < cols; ++j)
        {
            std::vector<Point3d> col;
            col.reserve(rows);
            for (std::size_t i = 0; i < rows; ++i) col.push_back(control_net[i][j]);
            py << "ax.plot(" << coordinates3_to_python(col,0) << ','
               << coordinates3_to_python(col,1) << ','
               << coordinates3_to_python(col,2)
               << ",'--o',linewidth=1.0,markersize=3)\n";
        }
    }
    append_common(py, options, true);
    PythonRuntime::instance().execute(py.str());
}
} // namespace opencagd::visualization
