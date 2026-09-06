#include <opencagd/visualization/matplotlib_figure.hpp>
#include <opencagd/visualization/python_runtime.hpp>

#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <utility>

namespace opencagd::visualization
{
namespace
{
std::string py_string(const std::string& input)
{
    std::string result = "'";
    for (char ch : input)
    {
        if (ch == '\\' || ch == '\'') result.push_back('\\');
        if (ch == '\n') result += "\\n"; else result.push_back(ch);
    }
    result.push_back('\'');
    return result;
}

template <typename Point>
std::string axis_values(const std::vector<Point>& points, std::size_t axis)
{
    std::ostringstream out;
    out << std::setprecision(17) << '[';
    for (std::size_t i = 0; i < points.size(); ++i)
    {
        if (i) out << ',';
        out << points[i][axis];
    }
    out << ']';
    return out.str();
}
} // namespace

MatplotlibFigure::MatplotlibFigure(AxisMode mode, PlotOptions options)
    : mode_(mode), options_(std::move(options)) {}

void MatplotlibFigure::require_mode(AxisMode expected) const
{
    if (mode_ != expected) throw std::logic_error("MatplotlibFigure axis mode mismatch");
}

void MatplotlibFigure::add_curve_2d(const std::vector<Point2d>& points, const std::string& label)
{
    require_mode(AxisMode::TwoD);
    if (points.empty()) return;
    body_ << "plt.plot(" << axis_values(points,0) << ',' << axis_values(points,1)
          << ",linewidth=2.0,label=" << py_string(label) << ")\n";
}

void MatplotlibFigure::add_curve_3d(const std::vector<Point3d>& points, const std::string& label)
{
    require_mode(AxisMode::ThreeD);
    if (points.empty()) return;
    body_ << "ax.plot(" << axis_values(points,0) << ',' << axis_values(points,1) << ',' << axis_values(points,2)
          << ",linewidth=1.8,label=" << py_string(label) << ")\n";
}

void MatplotlibFigure::add_control_polygon(const std::vector<Point2d>& points, const std::string& label)
{
    require_mode(AxisMode::TwoD);
    if (points.empty()) return;
    body_ << "plt.plot(" << axis_values(points,0) << ',' << axis_values(points,1)
          << ",'--o',linewidth=1.0,markersize=5,label=" << py_string(label) << ")\n";
}

void MatplotlibFigure::add_surface(const surface::SurfaceSampleGrid<Point3d>& grid, const std::string&)
{
    require_mode(AxisMode::ThreeD);
    if (grid.empty()) return;
    body_ << "X=np.array(" << axis_values(grid.points,0) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    body_ << "Y=np.array(" << axis_values(grid.points,1) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    body_ << "Z=np.array(" << axis_values(grid.points,2) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    body_ << "ax.plot_surface(X,Y,Z,alpha=0.80,linewidth=0,antialiased=True)\n";
}

void MatplotlibFigure::add_wireframe(const surface::SurfaceSampleGrid<Point3d>& grid)
{
    require_mode(AxisMode::ThreeD);
    if (grid.empty()) return;
    body_ << "X=np.array(" << axis_values(grid.points,0) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    body_ << "Y=np.array(" << axis_values(grid.points,1) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    body_ << "Z=np.array(" << axis_values(grid.points,2) << ").reshape(" << grid.u_count << ',' << grid.v_count << ")\n";
    body_ << "ax.plot_wireframe(X,Y,Z,rstride=4,cstride=4,linewidth=0.6)\n";
}

void MatplotlibFigure::add_control_net(const std::vector<std::vector<Point3d>>& net)
{
    require_mode(AxisMode::ThreeD);
    if (net.empty() || net.front().empty()) return;
    const std::size_t rows = net.size(), cols = net.front().size();
    for (const auto& row : net)
        if (row.size() != cols) throw std::invalid_argument("control net must be rectangular");
    for (const auto& row : net)
        body_ << "ax.plot(" << axis_values(row,0) << ',' << axis_values(row,1) << ',' << axis_values(row,2)
              << ",'--o',linewidth=1.0,markersize=3)\n";
    for (std::size_t j = 0; j < cols; ++j)
    {
        std::vector<Point3d> col;
        col.reserve(rows);
        for (std::size_t i = 0; i < rows; ++i) col.push_back(net[i][j]);
        body_ << "ax.plot(" << axis_values(col,0) << ',' << axis_values(col,1) << ',' << axis_values(col,2)
              << ",'--o',linewidth=1.0,markersize=3)\n";
    }
}

void MatplotlibFigure::add_isocurves(const surface::SurfaceSampleGrid<Point3d>& grid,
                                     std::size_t stride_u,
                                     std::size_t stride_v)
{
    require_mode(AxisMode::ThreeD);
    if (grid.empty()) return;
    stride_u = std::max<std::size_t>(1, stride_u);
    stride_v = std::max<std::size_t>(1, stride_v);
    for (std::size_t i = 0; i < grid.u_count; i += stride_u)
    {
        std::vector<Point3d> row;
        row.reserve(grid.v_count);
        for (std::size_t j = 0; j < grid.v_count; ++j) row.push_back(grid.at(i,j));
        body_ << "ax.plot(" << axis_values(row,0) << ',' << axis_values(row,1) << ',' << axis_values(row,2)
              << ",linewidth=0.7,alpha=0.65)\n";
    }
    for (std::size_t j = 0; j < grid.v_count; j += stride_v)
    {
        std::vector<Point3d> col;
        col.reserve(grid.u_count);
        for (std::size_t i = 0; i < grid.u_count; ++i) col.push_back(grid.at(i,j));
        body_ << "ax.plot(" << axis_values(col,0) << ',' << axis_values(col,1) << ',' << axis_values(col,2)
              << ",linewidth=0.7,alpha=0.65)\n";
    }
}

void MatplotlibFigure::render()
{
    std::ostringstream py;
    if (!options_.show) py << "import matplotlib\nmatplotlib.use('Agg')\n";
    py << "import matplotlib.pyplot as plt\n";
    if (mode_ == AxisMode::ThreeD) py << "import numpy as np\n";
    py << "fig=plt.figure(figsize=(8.2,6.4))\n";
    if (mode_ == AxisMode::ThreeD) py << "ax=fig.add_subplot(111,projection='3d')\n";
    py << body_.str();
    if (!options_.title.empty()) py << (mode_ == AxisMode::ThreeD ? "ax.set_title(" : "plt.title(") << py_string(options_.title) << ")\n";
    if (!options_.xlabel.empty()) py << (mode_ == AxisMode::ThreeD ? "ax.set_xlabel(" : "plt.xlabel(") << py_string(options_.xlabel) << ")\n";
    if (!options_.ylabel.empty()) py << (mode_ == AxisMode::ThreeD ? "ax.set_ylabel(" : "plt.ylabel(") << py_string(options_.ylabel) << ")\n";
    if (mode_ == AxisMode::ThreeD && !options_.zlabel.empty()) py << "ax.set_zlabel(" << py_string(options_.zlabel) << ")\n";
    if (options_.grid) py << (mode_ == AxisMode::ThreeD ? "ax.grid(True,alpha=0.3)\n" : "plt.grid(True,alpha=0.3)\n");
    if (options_.equal_axis) py << (mode_ == AxisMode::ThreeD ? "ax.set_box_aspect((1,1,1))\n" : "plt.axis('equal')\n");
    if (mode_ == AxisMode::ThreeD)
        py << "handles,labels=ax.get_legend_handles_labels()\nif handles: ax.legend(loc='best')\n";
    else
        py << "handles,labels=plt.gca().get_legend_handles_labels()\nif handles: plt.legend(loc='best')\n";
    py << "plt.tight_layout()\n";
    if (!options_.output_path.empty()) py << "plt.savefig(" << py_string(options_.output_path) << ",dpi=180,bbox_inches='tight')\n";
    py << (options_.show ? "plt.show()\n" : "plt.close()\n");
    PythonRuntime::instance().execute(py.str());
}
} // namespace opencagd::visualization
