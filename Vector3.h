#ifndef VECTOR3_H
#define VECTOR3_H

#include <Arduino.h>

struct Vector3 {
    float x, y, z;

    Vector3();
    Vector3(float x, float y, float z);

    // Vector math
    Vector3 operator+(const Vector3& other) const;
    Vector3& operator+=(const Vector3& other);
    Vector3 operator-(const Vector3& other) const;
    Vector3& operator-=(const Vector3& other);
    Vector3 operator/(float scalar) const;
    Vector3& operator/=(float scalar);
    Vector3 operator*(float scalar) const;
    Vector3& operator*=(float scalar);
    Vector3 operator*(int scalar) const;
    Vector3 operator*(double scalar) const;
    Vector3& operator*=(int scalar);
    float operator*(const Vector3& other) const; // dot product

    // Rotation
    void rotate(float pitch, float yaw, float roll);
    void rotate(const Vector3& rotation);

    // Normalization
    void normalize();
    Vector3 normalized() const;

    //Clamp
    void clamp(float clampRange);

    // Output
    String toString() const;
};

#endif