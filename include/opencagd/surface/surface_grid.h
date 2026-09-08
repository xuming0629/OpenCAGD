#pragma once

#include <cstddef>
#include <vector>
#include <opencagd/core/export.h>
#include <opencagd/geometry/point3d.h>

namespace opencagd {
namespace surface {

class OPENCAGD_SURFACE_API SurfaceSampleGrid
{
public:
    SurfaceSampleGrid();
    SurfaceSampleGrid(std::size_t u_count,
                      std::size_t v_count,
                      const std::vector<geometry::Point3D>& points);

    bool empty() const;
    std::size_t u_count() const;
    std::size_t v_count() const;
    const std::vector<geometry::Point3D>& points() const;
    const geometry::Point3D& at(std::size_t i, std::size_t j) const;

private:
    std::size_t u_count_;
    std::size_t v_count_;
    std::vector<geometry::Point3D> points_;
};

} // namespace surface
} // namespace opencagd
