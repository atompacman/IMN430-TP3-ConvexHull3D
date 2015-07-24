#ifndef __Point__
#define __Point__

#include <iostream>

#include "Vector.h"

class Point {
    
public:
        
    Point();
    Point(const double a_k);
    Point(double a_x, double a_y, double a_z);
    Point(const Point& a_p);
    
    ~Point();
    
    Point& operator=(const Point& a_p);
    Point operator-() const;
    Vector operator-(const Point& a_p) const;
    Point operator+(const Vector& a_v) const;
    Point operator-(const Vector& a_v) const;
    Point& operator+=(const Vector& a_v);
    Point& operator-=(const Vector& a_v);
    Point operator*(double a_k) const;
    Point operator/(double a_k) const;
    Point& operator*=(double a_k);
    Point& operator/=(double a_k);
    double operator[](int a_i) const;
    double& operator[](int a_i);
    
    friend double distance(const Point& a_p1, const Point& a_p2);
        
    double m_x;
    double m_y;
    double m_z;
};

inline Point Point::operator-() const
{
    return Point(-m_x, -m_y, -m_z);
}

inline Vector Point::operator-(const Point& a_p) const
{
    return Vector(m_x - a_p.m_x, m_y - a_p.m_y, m_z - a_p.m_z);
}

inline Point Point::operator+(const Vector& a_v) const
{
    return Point(m_x + a_v.m_x, m_y + a_v.m_y, m_z + a_v.m_z);
}

inline Point Point::operator-(const Vector& a_v) const
{
    return Point(m_x - a_v.m_x, m_y - a_v.m_y, m_z - a_v.m_z);
}

inline Point& Point::operator+=(const Vector& a_v)
{
    m_x += a_v.m_x;
    m_y += a_v.m_y;
    m_z += a_v.m_z;
    
    return *this;
}

inline Point& Point::operator-=(const Vector& a_v)
{
    m_x -= a_v.m_x;
    m_y -= a_v.m_y;
    m_z -= a_v.m_z;
    
    return *this;
}

inline Point Point::operator*(double a_k) const
{
    return Point(a_k * m_x, a_k * m_y, a_k * m_z);
}

inline Point Point::operator/(double a_k) const
{
    assert(a_k != 0.0f);
    
	float inv = (float)(1.0f / a_k);
    
    return Point(m_x * inv, m_y * inv, m_z * inv);
}

inline Point& Point::operator*=(double a_k)
{
    m_x *= a_k;
    m_y *= a_k;
    m_z *= a_k;
    
    return *this;
}

inline Point& Point::operator/=(double a_k)
{
    assert(a_k != 0.0f);
    
	float inv = (float)(1.0f / a_k);
    
    m_x *= inv;
    m_y *= inv;
    m_z *= inv;
    
    return *this;
}

inline double Point::operator[](int a_i) const
{
    assert(a_i >= 0 && a_i <= 2);
    
    return (&m_x)[a_i];
}

inline double& Point::operator[](int a_i)
{
    assert(a_i >= 0 && a_i <= 2);
    
    return (&m_x)[a_i];
}

inline double squareDistance(const Point& a_p1, const Point& a_p2)
{
    return (a_p1 - a_p2).squareNorm();
}

#endif /* defined(__RayTracer__Point__) */
