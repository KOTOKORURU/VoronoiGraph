#pragma once
#include <stdint.h>
#include <vector>
#include <fstream>
#include <string.h>
#include <iostream>
#include <limits>

#define D_MAX std::numeric_limits<double>::max()

struct Point2d
{
    static Point2d calculateCircleCenter(const Point2d& p1, const Point2d& p2, const Point2d& p3);
    static std::vector<Point2d> findParabolaIntersection(const Point2d& f1, const Point2d& f2, double directrix);

    Point2d(double x, double y)
        :
        x(x),
        y(y)
    {}

    Point2d()
        :
        x(0.0),
        y(0.0)
    {}

    Point2d(const Point2d& point)
        :
        x(point.x),
        y(point.y)
    {}

    void operator = (const Point2d& p2)
    {
        this->x = p2.x;
        this->y = p2.y;
    }

    Point2d operator+=(const Point2d& p2)
    {
        return { this->x + p2.x , this->y + p2.y };
    }
    Point2d operator /= (const double divisor)
    {
        return { this->x / divisor, this->y / divisor };
    }

    friend Point2d operator+(const Point2d& p1, const Point2d& p2)
    {
        return { p1.x + p2.x, p1.y + p2.y };
    }

    friend Point2d operator*(const Point2d& p, const double factor)
    {
        return { p.x * factor, p.y * factor};
    }

    friend Point2d operator*(const double factor, const Point2d& p)
    {
        return { p.x * factor, p.y * factor };
    }

    friend Point2d operator-(const Point2d& p1, const Point2d& p2)
    {
        return { p1.x - p2.x, p1.y - p2.y };
    }

    friend Point2d operator/(const Point2d& p1, const double divisor)
    {
        return { p1.x / divisor, p1.y / divisor };
    }

    friend bool operator==(const Point2d& p1, const Point2d& p2)
    {
        return ((p1.x == p2.x) && (p1.y == p2.y));
    }

    friend bool operator!=(const Point2d& p1, const Point2d& p2)
    {
       return !(p1==p2);
    }

    double norm()
    {
        return std::sqrt(x * x + y * y);
    }

    Point2d normalized()
    {
        return  (*this) / this->norm();
    }

    bool isValid()
    {
        return ((x != D_MAX) || (y != D_MAX));
    }

    double x;
    double y;
};