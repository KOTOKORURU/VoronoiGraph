#pragma once
#include<vector>
#include "misc.h"

class DrawAgent
{
public:
    DrawAgent() {}
    virtual ~DrawAgent() {}
    void DrawTheGraph(const std::vector<Point2d>& points, BoundingBox& bbox, std::vector<HalfEdge*>& vHalfEdges);

private:
    std::pair<Point2d, Point2d> CalculateRayPoint(const HalfEdge* pHalfEdge, const std::vector<Point2d>& points, BoundingBox& bbox);
    std::pair<Point2d, Point2d> CalculateRayIntersection(const Point2d& point1, const Point2d& point2,
        const Point2d& vertex, const BoundingBox& bbox, bool needTwoPoint);
};
