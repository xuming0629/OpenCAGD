#include <opencagd/geometry/vector3d.h>
#include <opencagd/math/numeric.h>

#include <cmath>
#include <stdexcept>

namespace opencagd {
namespace geometry {

Vector3D::Vector3D() : data_{0.0, 0.0, 0.0} {}
Vector3D::Vector3D(double x, double y, double z) : data_{x, y, z} {}

double Vector3D::x() const { return data_[0]; }
double Vector3D::y() const { return data_[1]; }
double Vector3D::z() const { return data_[2]; }

double Vector3D::operator[](std::size_t index) const
{
    if (index >= 3) throw std::out_of_range("Vector3D index out of range");
    return data_[index];
}

double& Vector3D::operator[](std::size_t index)
{
    if (index >= 3) throw std::out_of_range("Vector3D index out of range");
    return data_[index];
}

double Vector3D::squared_norm() const
{
    return dot(*this, *this);
}

double Vector3D::norm() const
{
    return std::sqrt(squared_norm());
}

Vector3D Vector3D::normalized() const
{
    const double length = norm();
    if (length <= math::tolerance())
        throw std::runtime_error("Cannot normalize a zero-length vector");
    return *this / length;
}

Vector3D& Vector3D::operator+=(const Vector3D& rhs)
{
    data_[0] += rhs.data_[0]; data_[1] += rhs.data_[1]; data_[2] += rhs.data_[2];
    return *this;
}

Vector3D& Vector3D::operator-=(const Vector3D& rhs)
{
    data_[0] -= rhs.data_[0]; data_[1] -= rhs.data_[1]; data_[2] -= rhs.data_[2];
    return *this;
}

Vector3D& Vector3D::operator*=(double scalar)
{
    data_[0] *= scalar; data_[1] *= scalar; data_[2] *= scalar;
    return *this;
}

Vector3D& Vector3D::operator/=(double scalar)
{
    if (std::abs(scalar) <= math::tolerance())
        throw std::invalid_argument("Vector3D division by zero");
    data_[0] /= scalar; data_[1] /= scalar; data_[2] /= scalar;
    return *this;
}

Vector3D operator+(Vector3D lhs, const Vector3D& rhs) { lhs += rhs; return lhs; }
Vector3D operator-(Vector3D lhs, const Vector3D& rhs) { lhs -= rhs; return lhs; }
Vector3D operator-(const Vector3D& value) { return Vector3D(-value.x(), -value.y(), -value.z()); }
Vector3D operator*(Vector3D value, double scalar) { value *= scalar; return value; }
Vector3D operator*(double scalar, Vector3D value) { value *= scalar; return value; }
Vector3D operator/(Vector3D value, double scalar) { value /= scalar; return value; }

double dot(const Vector3D& a, const Vector3D& b)
{
    return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

Vector3D cross(const Vector3D& a, const Vector3D& b)
{
    return Vector3D(
        a.y() * b.z() - a.z() * b.y(),
        a.z() * b.x() - a.x() * b.z(),
        a.x() * b.y() - a.y() * b.x());
}

} // namespace geometry
} // namespace opencagd
