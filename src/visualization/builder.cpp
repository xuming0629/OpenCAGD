#include <opencagd/visualization/builder.h>

#include <stdexcept>

namespace opencagd {
namespace visualization {

Curve2DData make_curve_2d_data(
    const std::vector<geometry::Point3D>& points,
    const std::vector<geometry::Point3D>& control_points,
    const std::string& label)
{
    if (points.empty())
        throw std::invalid_argument("curve points must not be empty");
    Curve2DData data;
    CurveSeries2D series;
    series.points = points;
    series.label = label;
    data.curves.push_back(series);
    data.control_points = control_points;
    return data;
}

Surface3DData make_surface_3d_data(
    const surface::SurfaceSampleGrid& grid,
    const std::vector<std::vector<geometry::Point3D> >& control_net)
{
    if (grid.empty() || grid.u_count() < 2 || grid.v_count() < 2)
        throw std::invalid_argument("surface grid must be at least 2 x 2");
    Surface3DData data;
    data.grid = grid;
    data.control_net = control_net;
    return data;
}

void append_surface_normals(
    Surface3DData& data,
    const surface::ParametricSurface& surface,
    std::size_t u_count,
    std::size_t v_count,
    double scale)
{
    if (u_count < 2 || v_count < 2)
        throw std::invalid_argument("normal grid must be at least 2 x 2");
    if (scale <= 0.0)
        throw std::invalid_argument("normal scale must be positive");

    const geometry::SurfaceDomain d = surface.domain();
    for (std::size_t i = 0; i < u_count; ++i)
    {
        const double au = static_cast<double>(i) / static_cast<double>(u_count - 1);
        const double u = d.u.min + d.u.length() * au;
        for (std::size_t j = 0; j < v_count; ++j)
        {
            const double av = static_cast<double>(j) / static_cast<double>(v_count - 1);
            const double v = d.v.min + d.v.length() * av;
            VectorGlyph3D glyph;
            glyph.origin = surface.evaluate(u, v);
            glyph.direction = surface.normal(u, v);
            glyph.scale = scale;
            data.vectors.push_back(glyph);
        }
    }
}

} // namespace visualization
} // namespace opencagd
