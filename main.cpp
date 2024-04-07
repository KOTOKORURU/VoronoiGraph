#include "include/voronoi.h"

// test point
std::vector<Point2d> points =
{ 
    {24, 21},
    {21, 29},
    {20, 28},
    {25, 25},
    {18, 27},
    {33, 35},
    {37, 30},
    {41, 40},
    {10, 23},
    {15, 40},
    {29, 31},
    {23, 42},
};
static BoundingBox bbox;

int main(void)
{
    bbox.min = Point2d(0.0, 0.0);
    bbox.max = Point2d(50.0, 50.0);

    VoronoiBuilder builder;
    builder.BuildVoronoiGraph(points);
    builder.DrawTheGraph(points, bbox);
}