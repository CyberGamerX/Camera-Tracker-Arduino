#include "Vector3.h"
#include <math.h>

// Constructors
Vector3::Vector3() : x(0), y(0), z(0) {}
Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

// Basic arithmetic operators
Vector3 Vector3::operator+(const Vector3& other) const {
    return Vector3(x + other.x, y + other.y, z + other.z);
}

Vector3& Vector3::operator+=(const Vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vector3 Vector3::operator-(const Vector3& other) const {
    return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3& Vector3::operator-=(const Vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Vector3 Vector3::operator/(float scalar) const {
    return Vector3(x / scalar, y / scalar, z / scalar);
}

Vector3& Vector3::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

Vector3 Vector3::operator*(float scalar) const {
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3& Vector3::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3 Vector3::operator*(int scalarI) const {
    float scalar = static_cast<float>(scalarI);
    return Vector3(x * scalar, y * scalar, z * scalar);
}

Vector3& Vector3::operator*=(int scalarI) {
    float scalar = static_cast<float>(scalarI);
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3 Vector3::operator*(double scalar) const {
    float s = static_cast<float>(scalar);
    return Vector3(x * s, y * s, z * s);
}

float Vector3::operator*(const Vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

// Normalization
void Vector3::normalize() {
    float mag = sqrt(x * x + y * y + z * z);
    if (mag > 0.0f) {
        x /= mag;
        y /= mag;
        z /= mag;
    }
}

Vector3 Vector3::normalized() const {
    float mag = sqrt(x * x + y * y + z * z);
    if (mag > 0.0f)
        return Vector3(x / mag, y / mag, z / mag);
    return Vector3(0, 0, 0);
}

//Clamp Range
void Vector3::clamp(float clampRange) {
    if (x < clampRange && x > -clampRange) x = 0;
    if (y < clampRange && y > -clampRange) y = 0;
    if (z < clampRange && z > -clampRange) z = 0;
}

// String output
String Vector3::toString() const {
    return "(" + String(x, 3) + ", " + String(y, 3) + ", " + String(z, 3) + ")";
}

// Rotation by pitch, yaw, roll in degrees
void Vector3::rotate(float pitch, float yaw, float roll) {
    float pitchRad = radians(pitch);
    float yawRad = radians(yaw);
    float rollRad = radians(roll);

    float cosPitch = cos(pitchRad);
    float sinPitch = sin(pitchRad);
    float cosYaw = cos(yawRad);
    float sinYaw = sin(yawRad);
    float cosRoll = cos(rollRad);
    float sinRoll = sin(rollRad);

    float newX = x * (cosYaw * cosRoll) + y * (cosYaw * sinRoll) + z * (-sinYaw);
    float newY = x * (sinPitch * sinYaw * cosRoll - cosPitch * sinRoll)
               + y * (sinPitch * sinYaw * sinRoll + cosPitch * cosRoll)
               + z * (sinPitch * cosYaw);
    float newZ = x * (cosPitch * sinYaw * cosRoll + sinPitch * sinRoll)
               + y * (cosPitch * sinYaw * sinRoll - sinPitch * cosRoll)
               + z * (cosPitch * cosYaw);

    x = newX;
    y = newY;
    z = newZ;

    normalize();
}

// Rotation by a Vector3 (interpreted as Euler angles)
void Vector3::rotate(const Vector3& rotation) {
    rotate(rotation.x, rotation.y, rotation.z);
}