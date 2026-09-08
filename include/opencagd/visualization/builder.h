#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/surface/parametric_surface.h>
#include <opencagd/visualization/data.h>

namespace opencagd {
namespace visualization {

OPENCAGD_VISUALIZATION_API Curve2DData make_curve_2d_data(
    const std::vector<geometry::Point3D>& points,
    const std::vector<geometry::Point3D>& control_points,
    const std::string& label = "curve");

OPENCAGD_VISUALIZATION_API Surface3DData make_surface_3d_data(
    const surface::SurfaceSampleGrid& grid,
    const std::vector<std::vector<geometry::Point3D> >& control_net);

OPENCAGD_VISUALIZATION_API void append_surface_normals(
    Surface3DData& data,
    const surface::ParametricSurface& surface,
    std::size_t u_count,
    std::size_t v_count,
    double scale);

} // namespace visualization
} // namespace opencagd
