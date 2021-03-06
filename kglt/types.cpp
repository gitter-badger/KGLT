#include <kazbase/random.h>
#include "types.h"

namespace kglt {

Radians to_radians(const Degrees& degrees) {
    return Radians(kmDegreesToRadians(degrees.value_));
}

Degrees to_degrees(const Radians& radians) {
    return Degrees(kmRadiansToDegrees(radians.value_));
}

kglt::Vec2 operator*(float lhs, const kglt::Vec2& rhs) {
    kglt::Vec2 result;
    kmVec2Scale(&result, &rhs, lhs);
    return result;
}

kglt::Vec3 operator*(float lhs, const kglt::Vec3& rhs) {
    kglt::Vec3 result;
    kmVec3Scale(&result, &rhs, lhs);
    return result;
}

kglt::Vec3 operator/(float lhs, const kglt::Vec3& rhs) {
    kglt::Vec3 result;
    kmVec3Scale(&result, &rhs, 1.0 / lhs);
    return result;
}

Vec3 Vec3::random_deviant(const Degrees& angle, const Vec3 up) const {
    //Lovingly adapted from ogre
    Vec3 new_up = (up == Vec3()) ? perpendicular() : up;

    Quaternion q;
    kmQuaternionRotationAxisAngle(&q, this, random_gen::random_float(0, 1) * (PI * 2.0));
    kmQuaternionMultiplyVec3(&new_up, &q, &new_up);
    kmQuaternionRotationAxisAngle(&q, &new_up, Radians(angle).value_);

    Vec3 ret;
    kmQuaternionMultiplyVec3(&ret, &q, this);

    return ret;
}

Vec3 Vec3::perpendicular() const {
    //Lovingly adapted from Ogre
    static const float square_zero = (float)(1e-06 * 1e-06);
    Vec3 perp = this->cross(Vec3(1, 0, 0));

    if(perp.length_squared() < square_zero) {
        //This vector is the X-axis, so use another
        perp = this->cross(Vec3(0, 1, 0));
    }
    return perp.normalized();
}

kglt::Vec3 operator-(const kglt::Vec3& vec) {
    return kglt::Vec3(-vec.x, -vec.y, -vec.z);
}

kglt::Quaternion operator-(const kglt::Quaternion& q) {
    return kglt::Quaternion(-q.x, -q.y, -q.z, -q.w);
}

std::ostream& operator<<(std::ostream& stream, const Vec2& vec) {
    stream << "(" << vec.x << "," << vec.y << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Vec3& vec) {
    stream << "(" << vec.x << "," << vec.y << "," << vec.z << ")";
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Quaternion& quat) {
    stream << "(" << quat.x << "," << quat.y << "," << quat.z << "," << quat.w << ")";
    return stream;
}

Quaternion Quaternion::look_rotation(const Vec3& direction, const Vec3& up=Vec3(0, 1, 0)) {
    Quaternion res;
    kmQuaternionLookRotation(&res, &direction, &up);
    return res;
}

Degrees::Degrees(const Radians &rhs) {
    value_ = kmRadiansToDegrees(rhs.value_);
}

Radians::Radians(const Degrees &rhs) {
    value_ = kmDegreesToRadians(rhs.value_);
}

}
