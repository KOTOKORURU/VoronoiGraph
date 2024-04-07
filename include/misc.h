#pragma once
#include <stdint.h>
#include "Point2d.h"
#include "Node.h"

class Node;
enum class EventType
{
    BasePoints = 0,
    Circle,
    Delete
};

struct BoundingBox
{
    Point2d min;
    Point2d max;
    bool isOutOfBbox(const Point2d& point) const
    {
        return (point.x < min.x) || (point.y < min.y) ||
            (point.x > max.x) || (point.y > max.y);
    };
};

class Event
{
public:
    Event(int index, EventType type, Node* pArc = nullptr)
        :
        m_pArc(pArc),
        m_eventType(type),
        m_index(index),
        m_eventPoint()
    {}
    ~Event() {};
    Node* m_pArc;
    EventType m_eventType;
    Point2d m_center;
    Point2d m_eventPoint;
    int m_index;
};

struct ComparePoint
{
    bool operator()(const std::shared_ptr<Event> pEvent1, const std::shared_ptr<Event> pEvent2)
    {
        return ((pEvent1->m_eventPoint.y == pEvent2->m_eventPoint.y &&
            (pEvent1->m_eventPoint.x > pEvent2->m_eventPoint.x)) || pEvent1->m_eventPoint.y > pEvent2->m_eventPoint.y);
    }
};

struct Vertex
{
    Vertex(Point2d* point2d)
        :
        m_Point2d(point2d)
    {
    }
    Vertex(const std::shared_ptr<Vertex>& vertex)
        :
        m_Point2d(vertex->m_Point2d)
    {}

    const Point2d GetPoint2d()
    {
        return *m_Point2d;
    }
    Point2d* m_Point2d;
};

class HalfEdge
{
public:
    static std::pair<HalfEdge*, HalfEdge*> makeHalfEdge(uint32_t leftIdx, uint32_t rightIdx);
    static void connectHalfEdge(HalfEdge* pFirst, HalfEdge* pNext);

    HalfEdge(uint32_t leftIdx = 0, uint32_t rightIdx = 0, Vertex* pVertex = nullptr)
        :
        leftIdx(leftIdx),
        rightIdx(rightIdx),
        pVertex(pVertex),
        pPrev(nullptr),
        pNext(nullptr),
        pTwin(nullptr)
    {
    }

    HalfEdge(const HalfEdge* pEdge)
        :
        leftIdx(pEdge->leftIdx),
        rightIdx(pEdge->rightIdx),
        pVertex(nullptr),
        pNext(nullptr),
        pPrev(nullptr),
        pTwin(nullptr)
    {
        if (pEdge->pVertex != nullptr)
        {
            pVertex = pEdge->pVertex;
        }

        if (pEdge->pNext != nullptr)
        {
            pNext = pEdge->pNext;
        }

        if (pEdge->pPrev != nullptr)
        {
            pPrev = pEdge->pPrev;
        }

        if (pEdge->pTwin != nullptr)
        {
            pTwin = pEdge->pTwin;
        }
    }

    HalfEdge operator = (const HalfEdge& edge)
    {
        this->pVertex = edge.pVertex;
        this->pNext = edge.pNext;
        this->pPrev = edge.pPrev;
        this->pTwin = edge.pTwin;
    }
    virtual~HalfEdge() {}

    HalfEdge* pPrev;
    HalfEdge* pNext;
    HalfEdge* pTwin;
    Vertex* pVertex;
    uint32_t leftIdx;
    uint32_t rightIdx;
};
