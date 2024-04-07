#include "../include/Node.h"

double Node::findXofTheArc(double sweepLine)
{
    double res = D_MAX;
    std::vector<Point2d> interPoints;
    if (m_pPoint1 != nullptr && m_pPoint2 != nullptr)
    {
        if (isLeaf())
        {
            res = m_pPoint2->x;
        }
        else
        {
            // Note:
            // 1. the x coord is increase in the return interPoints.
            // 2. the base Point "ij" means the i.y < j.y. 
            // 3. the arc "i" and arc "j" they have two intersection if they have different focus. 
            // 4. but if they have the same y coord in foucs, they have only one inertsection.
            // 
            // 5. So if the base Point is "ij", we then return the intersection point according the "ij" order.
            //    It is to say: the left side of the point is i, the right side is j.
            //    and the left intersection of "ij" arc, is "ij" point, the right intersection is "ji" Point.
            //
            // For example,if there is a event point k->x come to add to the tree:
            //
            //        (i,j)
            //        /   \
            //      (i,i) (j,i)
            //             /  \
            //          (j,j) (i,i)
            //
            // The k->x first compare to the intersection of (i,j), it's left intersection.
            // if k->x > the left intersection, it will then go to the (j,i), else it go to the (i,i)
            // just like AVL tree. assume: it's ture that (k->x > left intersection)
            // and k->x will then compare to the intersection of (j,i). it's right intersection.
            interPoints = Point2d::findParabolaIntersection(*m_pPoint1, *m_pPoint2, sweepLine);
            res = (m_pPoint1->y < m_pPoint2->y) ? interPoints[0].x : interPoints[1].x;
        }
    }
    return res;
}