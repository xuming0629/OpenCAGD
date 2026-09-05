#pragma once

#include <string>
#include <vector>

#include <opencagd/geometry/point.hpp>

namespace opencagd::visualization
{

struct PlotOptions
{
    std::string title;
    std::string xlabel = "x";
    std::string ylabel = "y";
    std::string output_path;
    bool equal_axis = false;
    bool grid = true;
    bool show = true;
};

void plot_curve_2d(
    const std::vector<Point2d>& curve_points,
    const std::vector<Point2d>& control_points,
    const PlotOptions& options);

void plot_basis_functions(
    const std::vector<double>& parameters,
    const std::vector<std::vector<double>>& basis_values,
    const std::vector<std::string>& labels,
    const PlotOptions& options);

} // namespace opencagd::visualization
