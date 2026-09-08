#include <opencagd/surface/surface_grid.h>

#include <stdexcept>

namespace opencagd {
namespace surface {

SurfaceSampleGrid::SurfaceSampleGrid() : u_count_(0), v_count_(0), points_() {}

SurfaceSampleGrid::SurfaceSampleGrid(std::size_t u_count,
                                     std::size_t v_count,
                                     const std::vector<geometry::Point3D>& points)
    : u_count_(u_count), v_count_(v_count), points_(points)
{
    if (u_count_ * v_count_ != points_.size())
        throw std::invalid_argument("SurfaceSampleGrid size does not match u_count * v_count");
}

bool SurfaceSampleGrid::empty() const { return points_.empty(); }
std::size_t SurfaceSampleGrid::u_count() const { return u_count_; }
std::size_t SurfaceSampleGrid::v_count() const { return v_count_; }
const std::vector<geometry::Point3D>& SurfaceSampleGrid::points() const { return points_; }

const geometry::Point3D& SurfaceSampleGrid::at(std::size_t i, std::size_t j) const
{
    if (i >= u_count_ || j >= v_count_)
        throw std::out_of_range("SurfaceSampleGrid index out of range");
    return points_[i * v_count_ + j];
}

} // namespace surface
} // namespace opencagd
