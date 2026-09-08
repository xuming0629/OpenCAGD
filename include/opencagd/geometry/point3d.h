#pragma once

#include <cstddef>
#include <opencagd/core/export.h>
#include <opencagd/geometry/vector3d.h>

namespace opencagd {
namespace geometry {

class OPENCAGD_CORE_API Point3D
{
public:
    Point3D();
    Point3D(double x, double y, double z = 0.0);

    double x() const;
    double y() const;
    double z() const;
    double operator[](std::size_t index) const;
    double& operator[](std::size_t index);

    Vector3D as_vector() const;

private:
    double data_[3];
};

OPENCAGD_CORE_API Point3D operator+(const Point3D& point, const Vector3D& vector);
OPENCAGD_CORE_API Point3D operator-(const Point3D& point, const Vector3D& vector);
OPENCAGD_CORE_API Vector3D operator-(const Point3D& a, const Point3D& b);
OPENCAGD_CORE_API Point3D lerp(const Point3D& a, const Point3D& b, double t);

} // namespace geometry
} // namespace opencagd
