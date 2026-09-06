#pragma once

#include <string>
#include <vector>

#include <opencagd/geometry/point.hpp>
#include <opencagd/surface/surface_grid.hpp>

namespace opencagd::visualization
{
struct PlotOptions
{
    std::string title;
    std::string xlabel;
    std::string ylabel;
    std::string zlabel;
    std::string output_path;
    bool grid{true};
    bool equal_axis{true};
    bool show{true};
};

void plot_curve_2d(
    const std::vector<Point2d>& curve_points,
    const std::vector<Point2d>& control_points = {},
    const PlotOptions& options = {});

void plot_basis_functions(
    const std::vector<double>& parameters,
    const std::vector<std::vector<double>>& basis_values,
    const std::vector<std::string>& labels,
    const PlotOptions& options = {});

void plot_surface_3d(
    const surface::SurfaceSampleGrid<Point3d>& grid,
    const std::vector<std::vector<Point3d>>& control_net = {},
    const PlotOptions& options = {});
} // namespace opencagd::visualization
