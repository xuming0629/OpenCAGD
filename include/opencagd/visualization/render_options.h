#pragma once

#include <string>
#include <opencagd/core/export.h>

namespace opencagd {
namespace visualization {

struct OPENCAGD_VISUALIZATION_API RenderOptions
{
    std::string title;
    std::string xlabel;
    std::string ylabel;
    std::string zlabel;
    std::string output_path;
    bool grid;
    bool equal_axis;
    bool show;
    int width;
    int height;

    RenderOptions();
};

} // namespace visualization
} // namespace opencagd
