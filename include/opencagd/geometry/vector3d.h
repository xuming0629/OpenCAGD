#pragma once

#include <cstddef>
#include <opencagd/core/export.h>

namespace opencagd {
namespace geometry {

class OPENCAGD_CORE_API Vector3D
{
public:
    Vector3D();
    Vector3D(double x, double y, double z);

    double x() const;
    double y() const;
    double z() const;
    double operator[](std::size_t index) const;
    double& operator[](std::size_t index);

    double squared_norm() const;
    double norm() const;
    Vector3D normalized() const;

    Vector3D& operator+=(const Vector3D& rhs);
    Vector3D& operator-=(const Vector3D& rhs);
    Vector3D& operator*=(double scalar);
    Vector3D& operator/=(double scalar);

private:
    double data_[3];
};

OPENCAGD_CORE_API Vector3D operator+(Vector3D lhs, const Vector3D& rhs);
OPENCAGD_CORE_API Vector3D operator-(Vector3D lhs, const Vector3D& rhs);
OPENCAGD_CORE_API Vector3D operator-(const Vector3D& value);
OPENCAGD_CORE_API Vector3D operator*(Vector3D value, double scalar);
OPENCAGD_CORE_API Vector3D operator*(double scalar, Vector3D value);
OPENCAGD_CORE_API Vector3D operator/(Vector3D value, double scalar);
OPENCAGD_CORE_API double dot(const Vector3D& a, const Vector3D& b);
OPENCAGD_CORE_API Vector3D cross(const Vector3D& a, const Vector3D& b);

} // namespace geometry
} // namespace opencagd
