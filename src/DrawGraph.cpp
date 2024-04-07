#include"../include/DrawGraph.h"
#include "../include/matplotlibcpp.h"

// Calculate the intersection of ray from vertex with bbox
std::pair<Point2d, Point2d> DrawAgent::CalculateRayIntersection(const Point2d& point1, const Point2d& point2,
    const Point2d& vertex, const BoundingBox& bbox, bool needTwoPoint)
{
     std::pair<Point2d,Point2d> res(Point2d(0,0), Point2d(0,0));
    double boxLine[4] = { bbox.min.x, bbox.max.x, bbox.min.y, bbox.max.y };

    double slop = - ((point1.x - point2.x) / (point1.y - point2.y));
    double ccw[2] = { -(point1.y - point2.y), point1.x - point2.x };

    // b = y - ax
    double intercept = vertex.y - slop * vertex.x;

    double boxX = ccw[0] > 0 ? bbox.max.x : bbox.min.x;
    double boxY = ccw[1] > 0 ? bbox.max.y : bbox.min.y;
    for (size_t i = 0; i < 2; i++)
    {
        Point2d point;
        if (i < 1)
        {
            double xLine = boxX;
            double yLine = (xLine * slop + intercept);
            point = Point2d(xLine, yLine);
        }
        else
        {
            double yLine = boxY;
            double xLine = (yLine - intercept) / slop;
            point = Point2d(xLine, yLine);
        }

        bool isOutofBound = bbox.isOutOfBbox(point);
        if (isOutofBound == false)
        {
            if (needTwoPoint == false)
            {
                res = { point, Point2d(0,0) };
            }
            else
            {
                if (res.first != Point2d(0, 0) && res.first != point)
                {
                    res.second = point;
                }
                else
                {
                    res.first = point;
                }
            }
        }
    }
    return res;
}

std::pair<Point2d, Point2d> DrawAgent::CalculateRayPoint(const HalfEdge* pHalfEdge,
    const std::vector<Point2d>& points, BoundingBox& bbox)
{
    const Point2d& edgePoint1 = points[pHalfEdge->leftIdx];
    const Point2d& edgePoint2 = points[pHalfEdge->rightIdx];
    Point2d point = pHalfEdge->pVertex->GetPoint2d();
    return CalculateRayIntersection(edgePoint1, edgePoint2, point, bbox, false);
}

void DrawAgent::DrawTheGraph(const std::vector<Point2d>& points, BoundingBox& bbox, std::vector<HalfEdge*>& vHalfEdges)
{
    std::vector<double> x = { 0, 0 };
    std::vector<double> y = { 0, 0 };
    for (size_t i = 0; i < points.size(); i++)
    {
        std::vector<double> x, y;
        x.push_back(points[i].x); y.push_back(points[i].y);
        matplotlibcpp::named_plot(std::to_string(i), x, y, ".");
    }

    for (size_t i = 0; i < vHalfEdges.size(); i++)
    {
        auto pHalfEdge = vHalfEdges[i];
        Vertex* pVertex;

        if (pHalfEdge->pVertex != nullptr && pHalfEdge->pTwin->pVertex != nullptr)
        {
            x[0] = pHalfEdge->pVertex->GetPoint2d().x;        y[0] = pHalfEdge->pVertex->GetPoint2d().y;
            x[1] = pHalfEdge->pTwin->pVertex->GetPoint2d().x; y[1] = pHalfEdge->pTwin->pVertex->GetPoint2d().y;

            matplotlibcpp::plot(x, y);
        }
        else if (pHalfEdge->pVertex != nullptr)
        {
            pVertex = pHalfEdge->pVertex;
            const Point2d& point1 = pHalfEdge->pVertex->GetPoint2d();
            const Point2d& point2 = CalculateRayPoint(pHalfEdge, points, bbox).first;

            x[0] = point1.x; y[0] = point1.y;
            x[1] = point2.x; y[1] = point2.y;
            matplotlibcpp::plot(x, y);
        }
        else if (pHalfEdge->pTwin->pVertex != nullptr)
        {
            pVertex = pHalfEdge->pTwin->pVertex;
            const Point2d& point1 = pHalfEdge->pTwin->pVertex->GetPoint2d();
            const Point2d& point2 = CalculateRayPoint(pHalfEdge->pTwin, points, bbox).first;

            x[0] = point1.x; y[0] = point1.y;
            x[1] = point2.x; y[1] = point2.y;
            matplotlibcpp::plot(x, y);
        }
        else // only two Point
        {
            const Point2d& edgePoint1 = points[pHalfEdge->leftIdx];
            const Point2d& edgePoint2 = points[pHalfEdge->rightIdx];
            Point2d vertex = 0.5 * (edgePoint1 + edgePoint2);
            if (edgePoint1.x == edgePoint2.x)
            {
                Point2d point1(bbox.min.x, vertex.y);
                Point2d point2(bbox.max.x, vertex.y);

                x[0] = point1.x; x[1] = point1.y;
                y[0] = point2.x; y[1] = point2.y;
                matplotlibcpp::plot(x, y);
            }
            else if (edgePoint1.y == edgePoint2.y)
            {
                double LineX = vertex.x;
                Point2d point1(vertex.x, bbox.min.y);
                Point2d point2(vertex.x, bbox.max.y);

                x[0] = point1.x; x[1] = point1.y;
                y[0] = point2.x; y[1] = point2.y;
                matplotlibcpp::plot(x, y);
            }
            else
            {
                std::pair<Point2d, Point2d> pointPair =
                    CalculateRayIntersection(edgePoint1, edgePoint2, vertex, bbox, true);

                x[0] = vertex.x; y[0] = vertex.y;
                x[1] = pointPair.first.x; y[1] = pointPair.first.y;
                matplotlibcpp::plot(x, y);
            }
        }
    }

    matplotlibcpp::xlim(bbox.min.x, bbox.max.x);
    matplotlibcpp::ylim(bbox.min.y, bbox.max.y);
    matplotlibcpp::show();
}