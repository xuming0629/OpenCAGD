#pragma once

#include <string>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/geometry/point3d.h>
#include <opencagd/geometry/vector3d.h>
#include <opencagd/surface/surface_grid.h>

namespace opencagd {
namespace visualization {

struct OPENCAGD_VISUALIZATION_API CurveSeries2D
{
    std::vector<geometry::Point3D> points;
    std::string label;
};

struct OPENCAGD_VISUALIZATION_API Curve2DData
{
    std::vector<CurveSeries2D> curves;
    std::vector<geometry::Point3D> control_points;
};

struct OPENCAGD_VISUALIZATION_API BasisFunctionData
{
    std::vector<double> parameters;
    std::vector<std::vector<double> > values;
    std::vector<std::string> labels;
};

struct OPENCAGD_VISUALIZATION_API DeCasteljauData
{
    std::vector<std::vector<geometry::Point3D> > levels;
    double parameter;
};

struct OPENCAGD_VISUALIZATION_API VectorGlyph3D
{
    geometry::Point3D origin;
    geometry::Vector3D direction;
    double scale;
};

struct OPENCAGD_VISUALIZATION_API Surface3DData
{
    surface::SurfaceSampleGrid grid;
    std::vector<std::vector<geometry::Point3D> > control_net;
    std::vector<VectorGlyph3D> vectors;
};

} // namespace visualization
} // namespace opencagd
