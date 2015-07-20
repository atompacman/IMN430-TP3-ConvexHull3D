//
//  Vector.cpp
//  RayTracer
//

#include <cmath>

#include "Vector.h"
#include "Point.h"

Vector::Vector()
:   m_x(0.0f),
    m_y(0.0f),
    m_z(0.0f)
{}

Vector::Vector(double a_k)
:   m_x(a_k),
    m_y(a_k),
    m_z(a_k)
{}

Vector::Vector(double a_x, double a_y, double a_z)
:   m_x(a_x),
    m_y(a_y),
    m_z(a_z)
{}

Vector::Vector(const Vector& a_v)
:   m_x(a_v.m_x),
    m_y(a_v.m_y),
    m_z(a_v.m_z)
{}

Vector::Vector(const Point& a_p)
:   m_x(a_p.m_x),
    m_y(a_p.m_y),
    m_z(a_p.m_z)
{}

Vector::~Vector()
{}

Vector& Vector::operator=(const Vector& a_v)
{
    if (this == &a_v)
    {
        return (*this);
    }
    
    m_x = a_v.m_x;
    m_y = a_v.m_y;
    m_z = a_v.m_z;
    
    return (*this);
}

Vector& Vector::operator=(const Point& a_p)
{
    m_x = a_p.m_x;
    m_y = a_p.m_y;
    m_z = a_p.m_z;
    
    return (*this);
}

double Vector::norm() const
{
    return sqrt(squareNorm());
}

Vector Vector::normalize() const
{
    float inv = (float) (1.0f / norm());
    
    return Vector(m_x*inv, m_y*inv, m_z*inv);
}