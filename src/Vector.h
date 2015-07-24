#ifndef __Vector__
#define __Vector__

#include <iostream>
#include <assert.h>
#include <cmath>

class Point;

class Vector {
    
public:
        
    Vector();
    Vector(double a_k);
    Vector(double a_x, double a_y, double a_z);
    Vector(const Vector& a_v);
    Vector(const Point& a_p);
    
    ~Vector();
    
    Vector& operator=(const Vector& a_v);
    Vector& operator=(const Point& a_p);
    Vector operator-() const;
    Vector operator*(double a_k) const;
    Vector operator/(double a_k) const;
    Vector& operator*=(double a_k);
    Vector& operator/=(double a_k);
    Vector operator+(const Vector& a_v) const;
    Vector operator-(const Vector& a_v) const;
    Vector& operator+=(const Vector& a_v);
    Vector& operator-=(const Vector& a_v);
    double operator[](int a_i) const;
    double& operator[](int a_i);
    
    double norm() const;
    double squareNorm() const;
    Vector normalize() const;
        
    double m_x;
    double m_y;
    double m_z;
};

inline Vector Vector::operator-() const
{
    return Vector(-m_x, -m_y, -m_z);
}

inline Vector Vector::operator*(double a_k) const
{
    return Vector(m_x * a_k, m_y * a_k, m_z * a_k);
}

inline Vector Vector::operator/(double a_k) const
{
    assert(a_k != 0.0f);
    
    double inv = 1.0f / a_k;
    
    return Vector(m_x * inv, m_y * inv, m_z * inv);
}

inline Vector& Vector::operator*=(double a_k)
{
    m_x *= a_k;
    m_y *= a_k;
    m_z *= a_k;
    
    return *this;
}

inline Vector& Vector::operator/=(double a_k)
{
    assert(a_k != 0.0f);
    
    double inv = 1.0f / a_k;
    
    m_x *= inv;
    m_y *= inv;
    m_z *= inv;
    
    return *this;
}

inline Vector Vector::operator+(const Vector& a_v) const
{
    return Vector(m_x + a_v.m_x, m_y + a_v.m_y, m_z + a_v.m_z);
}

inline Vector Vector::operator-(const Vector& a_v) const
{
    return Vector(m_x - a_v.m_x, m_y - a_v.m_y, m_z - a_v.m_z);
}

inline Vector& Vector::operator+=(const Vector& a_v)
{
    m_x += a_v.m_x;
    m_y += a_v.m_y;
    m_z += a_v.m_z;
    
    return *this;
}

inline Vector& Vector::operator-=(const Vector& a_v)
{
    m_x -= a_v.m_x;
    m_y -= a_v.m_y;
    m_z -= a_v.m_z;
    
    return *this;
}

inline double Vector::operator[](int a_i) const
{
    assert(a_i >= 0 && a_i <= 2);
    
    return (&m_x)[a_i];
}

inline double& Vector::operator[](int a_i)
{
    assert(a_i >= 0 && a_i <= 2);
    
    return (&m_x)[a_i];
}

inline double Vector::squareNorm() const
{
    return m_x*m_x + m_y*m_y + m_z*m_z;
}

// Non-member Inline Functions

inline double dot(const Vector& a_v1, const Vector& a_v2)
{
    return a_v1.m_x * a_v2.m_x + a_v1.m_y * a_v2.m_y + a_v1.m_z * a_v2.m_z;
}

inline double absDot(const Vector& a_v1, const Vector& a_v2)
{
    return std::abs(dot(a_v1, a_v2));
}

inline Vector cross(const Vector& a_v1, const Vector& a_v2)
{
    return Vector((a_v1.m_y * a_v2.m_z) - (a_v1.m_z * a_v2.m_y),
                  (a_v1.m_z * a_v2.m_x) - (a_v1.m_x * a_v2.m_z),
                  (a_v1.m_x * a_v2.m_y) - (a_v1.m_y * a_v2.m_x));
}

#endif
