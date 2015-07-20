//
//  Point.cpp
//  RayTracer
//

#include <assert.h>
#include <cmath>

#include "Point.h"

Point::Point()
:   m_x(0.0f),
    m_y(0.0f),
    m_z(0.0f)
{}

Point::Point(const double a_k)
:   m_x(a_k),
    m_y(a_k),
    m_z(a_k)
{}

Point::Point(double a_x, double a_y, double a_z)
:   m_x(a_x),
    m_y(a_y),
    m_z(a_z)
{}

Point::Point(const Point& a_p)
:   m_x(a_p.m_x),
    m_y(a_p.m_y),
    m_z(a_p.m_z)
{}

Point::~Point()
{}

Point& Point::operator=(const Point& a_p)
{
    if (this == &a_p)
    {
        return *this;
    }
    
    m_x = a_p.m_x;
    m_y = a_p.m_y;
    m_z = a_p.m_z;
    
    return *this;
}

double distance(const Point& a_p1, const Point& a_p2)
{
    return (a_p1 - a_p2).norm();
}