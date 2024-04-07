#pragma once
#include "misc.h"

class HalfEdge;
class Event;

class Node
{
public:
    Node(Point2d* p1, Point2d* p2, int pre_index, int this_index, double sweepLine)
        :m_pPoint1(p1), m_pPoint2(p2),
        m_indecies(pre_index, this_index),
        m_sweepline(sweepLine),
        m_pLeft(nullptr), m_pRight(nullptr),
        m_pPrev(nullptr), m_pNext(nullptr),
        m_pParent(nullptr), m_pEdge(nullptr)
    {}

    virtual~Node()
    {
    }

    bool isLeaf()
    {
        return m_indecies.first == m_indecies.second;
    }

    bool isNakeRoot()
    {
        return (m_pParent == nullptr) && (m_pLeft == nullptr) && (m_pRight == nullptr);
    }

    bool equals(const std::pair<int, int>& index)
    {
        return (this->m_indecies.first == index.first) &&
            (this->m_indecies.second == index.second);
    }

    double findXofTheArc(double sweepLine);

    std::pair<int, int> m_indecies;
    Node* m_pLeft, * m_pRight;
    Node* m_pPrev, * m_pNext;
    Node* m_pParent;
    double m_sweepline;

    Point2d* m_pPoint1;
    Point2d* m_pPoint2;
    HalfEdge* m_pEdge;
    std::shared_ptr<Event> m_pEvent;
};