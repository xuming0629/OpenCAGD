#include <opencagd/plot/matplotlib_renderer.h>
#include "python_runtime.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace opencagd {
namespace plot {
namespace {
std::string py_string(const std::string& input)
{
    std::string result = "'";
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        const char ch = input[i];
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

std::string coordinates_to_python(const std::vector<geometry::Point3D>& points,
                                  std::size_t axis)
{
    std::vector<double> values;
    values.reserve(points.size());
    for (std::size_t i = 0; i < points.size(); ++i)
        values.push_back(points[i][axis]);
    return doubles_to_python(values);
}

void append_backend(std::ostringstream& py, const visualization::RenderOptions& options)
{
    if (!options.show)
        py << "import matplotlib\nmatplotlib.use('Agg')\n";
    py << "import matplotlib.pyplot as plt\n";
}

void append_common(std::ostringstream& py,
                   const visualization::RenderOptions& o,
                   bool three_d)
{
    if (!o.title.empty()) py << (three_d ? "ax.set_title(" : "plt.title(") << py_string(o.title) << ")\n";
    if (!o.xlabel.empty()) py << (three_d ? "ax.set_xlabel(" : "plt.xlabel(") << py_string(o.xlabel) << ")\n";
    if (!o.ylabel.empty()) py << (three_d ? "ax.set_ylabel(" : "plt.ylabel(") << py_string(o.ylabel) << ")\n";
    if (three_d && !o.zlabel.empty()) py << "ax.set_zlabel(" << py_string(o.zlabel) << ")\n";
    if (o.grid) py << (three_d ? "ax.grid(True, alpha=0.3)\n" : "plt.grid(True, alpha=0.3)\n");
    if (!three_d && o.equal_axis) py << "plt.axis('equal')\n";
    if (three_d && o.equal_axis) py << "ax.set_box_aspect((1,1,1))\n";
    py << "plt.tight_layout()\n";
    if (!o.output_path.empty()) py << "plt.savefig(" << py_string(o.output_path) << ", dpi=180, bbox_inches='tight')\n";
    if (o.show) py << "plt.show()\n"; else py << "plt.close()\n";
}

void append_control_net(std::ostringstream& py,
                        const std::vector<std::vector<geometry::Point3D> >& net)
{
    if (net.empty()) return;
    const std::size_t rows = net.size();
    const std::size_t cols = net[0].size();
    for (std::size_t i = 0; i < rows; ++i)
    {
        if (net[i].size() != cols)
            throw std::invalid_argument("control net must be rectangular");
        py << "ax.plot(" << coordinates_to_python(net[i],0) << ','
           << coordinates_to_python(net[i],1) << ','
           << coordinates_to_python(net[i],2)
           << ",'--o',linewidth=1.0,markersize=3)\n";
    }
    for (std::size_t j = 0; j < cols; ++j)
    {
        std::vector<geometry::Point3D> column;
        for (std::size_t i = 0; i < rows; ++i) column.push_back(net[i][j]);
        py << "ax.plot(" << coordinates_to_python(column,0) << ','
           << coordinates_to_python(column,1) << ','
           << coordinates_to_python(column,2)
           << ",'--o',linewidth=1.0,markersize=3)\n";
    }
}
}

MatplotlibRenderer::MatplotlibRenderer() {}
MatplotlibRenderer::~MatplotlibRenderer() {}
std::string MatplotlibRenderer::name() const { return "matplotlib"; }

void MatplotlibRenderer::render(const visualization::Curve2DData& data,
                                const visualization::RenderOptions& options)
{
    if (data.curves.empty()) throw std::invalid_argument("curve data must not be empty");
    std::ostringstream py;
    append_backend(py, options);
    py << "plt.figure(figsize=(" << options.width / 120.0 << ',' << options.height / 120.0 << "))\n";
    for (std::size_t i = 0; i < data.curves.size(); ++i)
    {
        if (data.curves[i].points.empty()) throw std::invalid_argument("curve series must not be empty");
        py << "cx=" << coordinates_to_python(data.curves[i].points, 0) << "\n";
        py << "cy=" << coordinates_to_python(data.curves[i].points, 1) << "\n";
        py << "plt.plot(cx,cy,linewidth=2.0,label=" << py_string(data.curves[i].label) << ")\n";
    }
    if (!data.control_points.empty())
    {
        py << "px=" << coordinates_to_python(data.control_points, 0) << "\n";
        py << "py=" << coordinates_to_python(data.control_points, 1) << "\n";
        py << "plt.plot(px,py,'--o',linewidth=1.1,markersize=5,label='control polygon')\n";
    }
    py << "plt.legend()\n";
    append_common(py, options, false);
    PythonRuntime::instance().execute(py.str());
}

void MatplotlibRenderer::render(const visualization::BasisFunctionData& data,
                                const visualization::RenderOptions& options)
{
    if (data.parameters.empty() || data.values.empty())
        throw std::invalid_argument("basis data must not be empty");
    if (data.values.size() != data.labels.size())
        throw std::invalid_argument("basis values and labels size mismatch");
    std::ostringstream py;
    append_backend(py, options);
    py << "u=" << doubles_to_python(data.parameters) << "\n";
    py << "plt.figure(figsize=(" << options.width / 120.0 << ',' << options.height / 120.0 << "))\n";
    for (std::size_t i = 0; i < data.values.size(); ++i)
    {
        if (data.values[i].size() != data.parameters.size())
            throw std::invalid_argument("basis series length mismatch");
        py << "v=" << doubles_to_python(data.values[i]) << "\n";
        py << "plt.plot(u,v,linewidth=1.8,label=" << py_string(data.labels[i]) << ")\n";
    }
    py << "plt.legend()\n";
    append_common(py, options, false);
    PythonRuntime::instance().execute(py.str());
}

void MatplotlibRenderer::render(const visualization::DeCasteljauData& data,
                                const visualization::RenderOptions& options)
{
    if (data.levels.empty() || data.levels[0].empty())
        throw std::invalid_argument("De Casteljau levels must not be empty");
    std::ostringstream py;
    append_backend(py, options);
    py << "plt.figure(figsize=(" << options.width / 120.0 << ',' << options.height / 120.0 << "))\n";
    for (std::size_t level = 0; level < data.levels.size(); ++level)
    {
        py << "x=" << coordinates_to_python(data.levels[level],0) << "\n";
        py << "y=" << coordinates_to_python(data.levels[level],1) << "\n";
        if (level == 0)
            py << "plt.plot(x,y,'--o',linewidth=1.4,markersize=6,label='P_i')\n";
        else if (level + 1 == data.levels.size())
            py << "plt.plot(x,y,'o',markersize=9,label='C(t)')\n";
        else
            py << "plt.plot(x,y,'--o',linewidth=1.0,markersize=5,label='level " << level << "')\n";
    }
    py << "plt.legend()\n";
    append_common(py, options, false);
    PythonRuntime::instance().execute(py.str());
}

void MatplotlibRenderer::render(const visualization::Surface3DData& data,
                                const visualization::RenderOptions& options)
{
    if (data.grid.empty() || data.grid.u_count() < 2 || data.grid.v_count() < 2)
        throw std::invalid_argument("surface grid must be at least 2 x 2");
    const std::vector<geometry::Point3D>& points = data.grid.points();
    std::vector<double> x, y, z;
    for (std::size_t i = 0; i < points.size(); ++i)
    {
        x.push_back(points[i].x()); y.push_back(points[i].y()); z.push_back(points[i].z());
    }
    std::ostringstream py;
    append_backend(py, options);
    py << "import numpy as np\n";
    py << "x=np.array(" << doubles_to_python(x) << ").reshape(" << data.grid.u_count() << ',' << data.grid.v_count() << ")\n";
    py << "y=np.array(" << doubles_to_python(y) << ").reshape(" << data.grid.u_count() << ',' << data.grid.v_count() << ")\n";
    py << "z=np.array(" << doubles_to_python(z) << ").reshape(" << data.grid.u_count() << ',' << data.grid.v_count() << ")\n";
    py << "fig=plt.figure(figsize=(" << options.width / 120.0 << ',' << options.height / 120.0 << "))\nax=fig.add_subplot(111,projection='3d')\n";
    py << "ax.plot_surface(x,y,z,alpha=0.78,linewidth=0,antialiased=True)\n";
    append_control_net(py, data.control_net);
    if (!data.vectors.empty())
    {
        std::vector<double> px,pyv,pz,nx,ny,nz,scales;
        for (std::size_t i = 0; i < data.vectors.size(); ++i)
        {
            px.push_back(data.vectors[i].origin.x()); pyv.push_back(data.vectors[i].origin.y()); pz.push_back(data.vectors[i].origin.z());
            nx.push_back(data.vectors[i].direction.x() * data.vectors[i].scale);
            ny.push_back(data.vectors[i].direction.y() * data.vectors[i].scale);
            nz.push_back(data.vectors[i].direction.z() * data.vectors[i].scale);
        }
        py << "px=" << doubles_to_python(px) << "\npy=" << doubles_to_python(pyv) << "\npz=" << doubles_to_python(pz) << "\n";
        py << "nx=" << doubles_to_python(nx) << "\nny=" << doubles_to_python(ny) << "\nnz=" << doubles_to_python(nz) << "\n";
        py << "ax.quiver(px,py,pz,nx,ny,nz,normalize=False,linewidth=0.9)\n";
    }
    append_common(py, options, true);
    PythonRuntime::instance().execute(py.str());
}

} // namespace plot
} // namespace opencagd
