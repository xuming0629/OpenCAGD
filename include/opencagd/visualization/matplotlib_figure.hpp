#pragma once

#include <sstream>
#include <string>
#include <vector>

#include <opencagd/geometry/point.hpp>
#include <opencagd/surface/surface_grid.hpp>
#include <opencagd/visualization/matplotlib.hpp>

namespace opencagd::visualization
{
class MatplotlibFigure
{
public:
    enum class AxisMode { TwoD, ThreeD };

    explicit MatplotlibFigure(AxisMode mode, PlotOptions options = {});

    void add_curve_2d(const std::vector<Point2d>& points,
                      const std::string& label = "curve");
    void add_curve_3d(const std::vector<Point3d>& points,
                      const std::string& label = "curve");
    void add_vectors_3d(const std::vector<Point3d>& origins,
                        const std::vector<Point3d>& vectors,
                        double length = 0.25,
                        const std::string& label = "vectors");
    void add_control_polygon(const std::vector<Point2d>& points,
                             const std::string& label = "control polygon");
    void add_vectors_2d(const std::vector<Point2d>& origins,
                        const std::vector<Point2d>& vectors,
                        double scale = 0.25,
                        const std::string& label = "vectors");
    void add_surface(const surface::SurfaceSampleGrid<Point3d>& grid,
                     const std::string& label = "surface");
    void add_wireframe(const surface::SurfaceSampleGrid<Point3d>& grid);
    void add_control_net(const std::vector<std::vector<Point3d>>& control_net);
    void add_isocurves(const surface::SurfaceSampleGrid<Point3d>& grid,
                       std::size_t stride_u = 5,
                       std::size_t stride_v = 5);

    void render();

private:
    void require_mode(AxisMode expected) const;
    AxisMode mode_;
    PlotOptions options_;
    std::ostringstream body_;
};
} // namespace opencagd::visualization
