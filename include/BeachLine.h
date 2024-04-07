#pragma once
#include <vector>
#include "Point2d.h"
#include "misc.h"
#include "Node.h"

#define Destructor(Pointer) \
    if(Pointer != nullptr) \
    {\
       delete Pointer; \
       Pointer = nullptr; \
    }

class HalfEdge;
class Node;

class BeachLine
{
public:
    BeachLine() 
        :
        m_pRoot(nullptr),
        m_pLeftLeafNow(nullptr),
        m_pRightLeafNow(nullptr)
    {
    };

    BeachLine(const uint32_t& index, std::vector<Point2d>& points, double xValue) 
        :
        m_pRoot(nullptr),
        m_pLeftLeafNow(nullptr),
        m_pRightLeafNow(nullptr)
    {
        if (m_pRoot == nullptr)
            m_pRoot = CreateNode(index, index, points, xValue);
    };

    virtual ~BeachLine()
    {
        for (size_t i = 0; i < nodeList.size(); i++)
        {
            Node* pNode = nodeList[i];
            Destructor(pNode)
        }
    }

    std::pair<Node*, Node*> GetLeafNodePair()
    {
        return std::make_pair(m_pLeftLeafNow, m_pRightLeafNow);
    }

    void AddNode(uint32_t index, std::vector<Point2d>& points, double sweepline, std::vector<HalfEdge*>& vHalfEdges);
    void RemoveNode(Node** node, std::vector<Point2d>& points);
    Node* GetRoot() { return m_pRoot; }
    std::pair<Node*, Node*> breakArc(Node* leafNode);
    std::shared_ptr<Event> CheckCircleEvent(Node* pLeftLeaf, Node* pMidleLeaf, Node* pRightLeaf, double sweepLine);

private:
    void connect(Node* pPrev, Node* pNext)
    {
        pPrev->m_pNext = pNext;
        pNext->m_pPrev = pPrev;
    }

    void LeftRotate(Node** root);
    void RightRotate(Node** root);
    Node* BalanceTree(Node* root);
    Node* CreateNode(uint32_t prevIndex, uint32_t index, std::vector<Point2d>& points, double xValue);
    void AddNode(Node* pRootNode, uint32_t index, std::vector<Point2d>& points, double xValue, std::vector<HalfEdge*>& vHalfEdges);
    Node* MakeSubTree(uint32_t prevIndex, uint32_t index, std::vector<Point2d>& points, double sweepline, std::vector<HalfEdge*>& vHalfEdgess);
    int GetNodeHeight(Node* node);
    int GetNodeRightHeight(Node* node);
    int GetNodeLeftHeight(Node* node);
    Node* deleteNode(Node** node, std::vector<Point2d>& points);
    Node* FindNode(Node* pNode, double xValue, double sweepLine);

    std::vector<HalfEdge> resEdgeVector;
    std::vector<Node*> nodeList;

    Node* m_pRoot;
    Node* m_pLeftLeafNow;
    Node* m_pRightLeafNow;
};