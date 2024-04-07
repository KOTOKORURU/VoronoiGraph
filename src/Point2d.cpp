#include <cassert>
#include "../include/Point2d.h"

//std::pair<double, double> GetLineFunc(const Point2d& p1, const Point2d& p2)
//{
//    auto midpoint = [&](Point2d p1, Point2d p2) -> Point2d
//    {
//        return (p1 + p2) / 2;
//    };
//
//    Point2d mid = midpoint(p1, p2);
//    double slop = D_MAX;
//    double intercept = mid.x;
//    if ((p1.y - p2.y) != 0)
//    {
//        slop = -(p2.y - p1.y) / (p2.x - p1.x);
//        intercept = mid.y - mid.x * slop;
//    }
//
//    return { slop, intercept };
//}

Point2d Point2d::calculateCircleCenter(const Point2d& p1, const Point2d& p2, const Point2d& p3)
{
    Point2d center;
    double x1 = p1.x, x2 = p2.x, x3 = p3.x;
    double y1 = p1.y, y2 = p2.y, y3 = p3.y;

    /*
    * u1  = (x2^2 - x1^2 + y2^2 - y1^2) / 2
    * u2  = (x3^2 - x1^2 + y3^2 - y1^2) / 2
    * d11 = x2 - x1, d12 = y2 - y1
    * d21 = x3 - x1, d22 = y3 - y1
    * centerX = (u1 * d22 - u2 * d12) / (d11 * d22 - d21 * d12)
    * centerY = (u2 * d11 - u1 * d21) / (d11 * d22 - d21 * d12)
    */
    double d11 = x2 - x1, d12 = y2 - y1;
    double d21 = x3 - x1, d22 = y3 - y1;
    double u1 = (x2 * x2 - x1 * x1 + y2 * y2 - y1 * y1) / 2;
    double u2 = (x3 * x3 - x1 * x1 + y3 * y3 - y1 * y1) / 2;

    double det = d11 * d22 - d21 * d12;
    if (fabs(det) < 1e-5)
    {
        return Point2d(D_MAX, D_MAX);
    }
    center.x = (u1 * d22 - u2 * d12) / det;
    center.y = (u2 * d11 - u1 * d21) / det;
    return center;
}

// Two base point as the focus, the sweep line as the directrix.
std::vector<Point2d> Point2d::findParabolaIntersection(const Point2d& f1, const Point2d& f2, double directrix)
{
    std::vector<Point2d> res;
    double x1 = f1.x, y1 = f1.y;
    double x2 = f2.x, y2 = f2.y;
    double d = directrix;

    double A = 2 * (y1 - d);
    double B = 2 * (y2 - d);
    double C = x1 * x1 + y1 * y1 - d * d;
    double D = x2 * x2 + y2 * y2 - d * d;
    double AB = A * B;

    double a = (B - A) / AB;
    double negb = -2 * (A * x2 - B * x1) / AB;
    double c = (B * C - A * D) / AB;
    double b2 = negb * negb;
    double deltapow = b2 - 4 * a * c;
    double a2 = a * 2;

    if (deltapow == 0)
    {
        double intersectionX = negb / a2;
        double intersectionY = intersectionX * intersectionX + C - 2 * intersectionX * x1;
        intersectionY /= 2 * (y1 - d);
        res.push_back({ intersectionX, intersectionY });
    }
    else
    {
        double intersectionX1 = (negb + std::sqrt(deltapow)) / a2;
        double intersectionX2 = (negb - std::sqrt(deltapow)) / a2;

        double intersectionY1 = intersectionX1 * intersectionX1 + C - 2 * intersectionX1 * x1;
        double intersectionY2 = intersectionX2 * intersectionX2 + C - 2 * intersectionX2 * x1;
        intersectionY1 /= 2 * (y1 - d);
        intersectionY2 /= 2 * (y1 - d);
        if (intersectionX1 > intersectionX2)
        {
            std::swap(intersectionX1, intersectionX2);
            std::swap(intersectionY1, intersectionY2);
        }
        res.push_back({ intersectionX1, intersectionY1 });
        res.push_back({ intersectionX2, intersectionY2 });
    }
    return res;
}