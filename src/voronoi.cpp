#include <queue>
#include <cassert>

#include "../include/voronoi.h"
#include "../include/misc.h"

#define BREAKPOINTS_INTERSECTION_EPSILON 1.0e-5
static std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event> >, ComparePoint> eventQueue;

void InitEventQueue(const std::vector<Point2d>& vec)
{
    for (size_t i = 0; i < vec.size(); i++)
    {
        const Point2d& point = vec[i];
        auto pEvent = std::make_shared<Event>(i, EventType::BasePoints);
        pEvent->m_eventPoint = point;
        eventQueue.push(pEvent);
    }
}

void VoronoiBuilder::BuildVoronoiGraph(std::vector<Point2d>& points)
{
    InitEventQueue(points);
    BeachLine* pBeachLine = GetBeachLine();

    while (eventQueue.empty() == false)
    {
        auto pEvent = eventQueue.top();
        eventQueue.pop();
        int index = pEvent->m_index;
        double sweepLine = pEvent->m_eventPoint.y;
        double xValue = pEvent->m_eventPoint.x;
        if (pEvent->m_eventType == EventType::BasePoints)
        {
            if (pBeachLine == nullptr)
            {
                pBeachLine = new BeachLine(index, points, sweepLine);
                SetBeachLine(pBeachLine);
            }
            else
            {
                pBeachLine->AddNode(index, points, xValue, m_VhalfEdges);
                Node* pLeftLeaf = pBeachLine->GetLeafNodePair().first;
                Node* pRightLeaf = pBeachLine->GetLeafNodePair().second;
                // new arc on the right.
                auto pCircleEventLeft = pBeachLine->CheckCircleEvent(pLeftLeaf->m_pPrev, pLeftLeaf, pLeftLeaf->m_pNext, sweepLine);
                if (pCircleEventLeft != nullptr)
                {
                    eventQueue.push(pCircleEventLeft);
                }
                // new arc on the left.
                auto pCircleEventRight = pBeachLine->CheckCircleEvent(pRightLeaf->m_pPrev, pRightLeaf, pRightLeaf->m_pNext, sweepLine);
                if (pCircleEventRight != nullptr)
                {
                    eventQueue.push(pCircleEventRight);
                }
            }
        }
        else if (pEvent->m_eventType == EventType::Circle)
        {
            // Assume the arc tuple is i<->j<->k. j is the node te be deleted
            Node* pNode = pEvent->m_pArc;
            BeachLine* pBeachLine = GetBeachLine();

            // get the parent and grandParent like: ij, jk.
            std::pair<Node*, Node*> parentPair = pBeachLine->breakArc(pNode);

            if (parentPair.first == nullptr || parentPair.second == nullptr)
                continue;

            // there will be two groups intersection of three arc tuple(each arc may have two intersctions),
            // one group will be merged to one point, another will not, it is a false alert. 
            // for example the i, j, k arc, if the i is vertical above the j, 
            // the intersections of i and j will be (ij, ji), the intersection of jk will be (jk, kj).
            // So one group intersection is (ij, jk).
            // another group intersection is (ji, kj)
            double intersectionX1 = parentPair.first->findXofTheArc(sweepLine);
            double intersectionX2 = parentPair.second->findXofTheArc(sweepLine);
            double delta = std::fabs(intersectionX1 - intersectionX2);
            if (delta > BREAKPOINTS_INTERSECTION_EPSILON)
                continue;
            ;

            // Get Circle center.
            // Get (i -> j) edge.
            // Get (j -> k) edge.
            Point2d center = pEvent->m_center;
            HalfEdge* pEdge1 = parentPair.first->m_pEdge;
            HalfEdge* pEdge2 = parentPair.second->m_pEdge;

            Node* pPrevLeaf = pNode->m_pPrev;
            Node* pNextLeaf = pNode->m_pNext;
            assert(pPrevLeaf != nullptr);
            assert(pNextLeaf != nullptr);
            if (pPrevLeaf->m_pEvent != nullptr && pPrevLeaf != nullptr)
            {
                pPrevLeaf->m_pEvent->m_eventType = EventType::Delete;
            }
            if (pNextLeaf->m_pEvent != nullptr && pNextLeaf != nullptr)
            {
                pNextLeaf->m_pEvent->m_eventType = EventType::Delete;
            }

            Node* pParent = pNode->m_pParent;
            Node* pNewEdgeNode = nullptr;
            if (pParent == parentPair.first)
            {
                pNewEdgeNode = parentPair.second;
            }
            else
            {
                pNewEdgeNode = parentPair.first;
            }

            // Remove this event arc Node.
            pBeachLine->RemoveNode(&pNode, points);

            // new edge: i<->k
            std::pair<HalfEdge*, HalfEdge*> pNewEdgePair = HalfEdge::makeHalfEdge(pPrevLeaf->m_indecies.first, pNextLeaf->m_indecies.first);

            pNewEdgeNode->m_pEdge = pNewEdgePair.first;

            // Seems we don't need this, because we iterate edge vector instead of edge
            // connect the new edge to those two old edges.
            // (i -> j) <-> (i -> k).
            // (j -> k) <-> (j -> i) 
            // (k -> i) <-> (k -> j)
            //HalfEdge::connectHalfEdge(pEdge1, pNewEdgePair.first);
            //HalfEdge::connectHalfEdge(pEdge2, pEdge1->pTwin);
            //HalfEdge::connectHalfEdge(pNewEdgePair.second, pEdge2->pTwin);

            // update the new vertex to those two new edge.
            // vertex -> (i<->k),
            // (i->k) -> vertex.
            Point2d* centerPtr = new Point2d(center);
            Vertex* pVertex = new Vertex(centerPtr);
            pNewEdgePair.second->pVertex = pVertex;

            // recode the new vertex.
            m_Vertices.push_back(pVertex);

            // update the new vertex to those two old edge.
            // (i -> j) -> vertex.
            // (j -> k) -> vertex.
            pEdge1->pVertex = pVertex;
            pEdge2->pVertex = pVertex;

            m_VhalfEdges.push_back(pNewEdgePair.first);
            //vHalfEdges.push_back(pNewEdgePair.second);

            // check again the circle event, after remove the "j" arc.
            if (pPrevLeaf != nullptr && pNextLeaf != nullptr)
            {
                // i.prev<->i<->k
                auto pCircleEventLeft = pBeachLine->CheckCircleEvent(pPrevLeaf->m_pPrev, pPrevLeaf, pNextLeaf, sweepLine);
                if (pCircleEventLeft != nullptr)
                {
                    eventQueue.push(pCircleEventLeft);
                }
                // i<->k<->k.next
                auto pCircleEventRight = pBeachLine->CheckCircleEvent(pPrevLeaf, pNextLeaf, pNextLeaf->m_pNext, sweepLine);
                if (pCircleEventRight != nullptr)
                {
                    eventQueue.push(pCircleEventRight);
                }
            }
        }
    }
}