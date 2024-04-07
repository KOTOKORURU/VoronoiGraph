#include <limits>
#include "../include/BeachLine.h"
#include "../include/matplotlibcpp.h"

#define D_MAX std::numeric_limits<double>::max()

std::shared_ptr<Event> BeachLine::CheckCircleEvent(Node* pLeftLeaf, Node* pMidleLeaf, Node* pRightLeaf, double sweepLine)
{
    std::shared_ptr<Event> pRes = nullptr;
    if (pLeftLeaf != nullptr && pMidleLeaf != nullptr && pRightLeaf != nullptr)
    {
        Point2d* pPointLeft = pLeftLeaf->m_pPoint1;
        Point2d* pPointMidle = pMidleLeaf->m_pPoint2;
        Point2d* pPointRight = pRightLeaf->m_pPoint1;

        // the new arc shouldn't be in the middle, The prev and next arc of new arc is from the same arc.
        if (pMidleLeaf->m_pPrev != pMidleLeaf->m_pNext)
        {
            Point2d center = Point2d::calculateCircleCenter(*pPointLeft, *pPointMidle, *pPointRight);
            if (center.isValid())
            {
                // Get radius of the circle.
                double R = (center - *pPointLeft).norm();
                //For 'equal'check sometime the sweepLine is smaller than R, when the line just lay in the basePoint.
                if ((center.y + R) == sweepLine || (center.y + R) > sweepLine)
                {
                    // event -> leaf
                    auto pEvent = std::make_shared<Event>(pMidleLeaf->m_indecies.first, EventType::Circle, pMidleLeaf);
                    // when sweepline comes(sweepline == center.y + R), 
                    // this means the distance of three points to the center point equals to the center to sweepLine
                    // the Event is truely happend, so add the point to the sweepline.
                    // this point is sweepline actually lay on.
                    Point2d eventPoint = center;
                    eventPoint.y += R;
                    pEvent->m_center = center;
                    pEvent->m_eventPoint = eventPoint;
                    // leaf -> event
                    pMidleLeaf->m_pEvent = pEvent;
                    pRes = pEvent;
                }
            }
        }
    }
    return pRes;
}

Node* BeachLine::MakeSubTree(uint32_t prevIndex, uint32_t index, std::vector<Point2d>& points, double sweepline, std::vector<HalfEdge*>& vHalfEdges)
{
    Point2d* point1 = &points[prevIndex];
    Point2d* point2 = &points[index];
    Node* res = nullptr;
    if (*point1 != *point2)
    {
        Node* node1 = CreateNode(prevIndex, index, points, sweepline);
        Node* node2 = CreateNode(index, prevIndex, points, sweepline);

        Node* pLeftLeaf = CreateNode(prevIndex, prevIndex, points, sweepline);
        Node* pRightLeaf = CreateNode(prevIndex, prevIndex, points, sweepline);
        Node* pMidleLeaf = CreateNode(index, index, points, sweepline);

        std::pair<HalfEdge*, HalfEdge*> pTwinEdge = HalfEdge::makeHalfEdge(prevIndex, index);

        node1->m_pRight = node2;
        node1->m_pLeft = pLeftLeaf;
        node1->m_pEdge = pTwinEdge.first;

        node2->m_pRight = pRightLeaf;
        node2->m_pLeft = pMidleLeaf;
        node2->m_pParent = node1;
        node2->m_pEdge = pTwinEdge.second;

        pLeftLeaf->m_pParent = node1;
        pMidleLeaf->m_pParent = node2;
        pRightLeaf->m_pParent = node2;

        vHalfEdges.push_back(pTwinEdge.first);
        //vHalfEdges.push_back(pTwinEdge.second);

        connect(pLeftLeaf, pMidleLeaf);
        connect(pMidleLeaf, pRightLeaf);
        m_pLeftLeafNow = pLeftLeaf;
        m_pRightLeafNow = pRightLeaf;

        res = node1;
    }
    else if (point1->y == point2->y && point1->x != point2->x)
    {
        uint32_t first = 0;
        uint32_t second = 0;
        if (point1->x < point2->x)
        {
            first = prevIndex;
            second = index;
        }
        else
        {
            first = index;
            second = prevIndex;
        }
        std::pair<HalfEdge*, HalfEdge*> pTwinEdge = HalfEdge::makeHalfEdge(first, second);
        Node* node = CreateNode(first, second, points, sweepline);
        Node* pLeftLeaf = CreateNode(first, first, points, sweepline);
        Node* pRightLeaf = CreateNode(second, second, points, sweepline);
        node->m_pLeft = pLeftLeaf;
        node->m_pRight = pRightLeaf;
        pLeftLeaf->m_pParent = node;
        pRightLeaf->m_pParent = node;

        node->m_pEdge = pTwinEdge.first;
        vHalfEdges.push_back(pTwinEdge.first);
        // vHalfEdges.push_back(pTwinEdge.second);
        connect(pLeftLeaf, pRightLeaf);
        m_pLeftLeafNow = pLeftLeaf;
        m_pRightLeafNow = pRightLeaf;
        res = node;
    }
    return res;
}

std::pair<Node*, Node*> BeachLine::breakArc(Node* leafNode)
{
    std::pair<Node*, Node*> res;
    std::pair<int, int> break1 = std::make_pair(leafNode->m_pPrev->m_indecies.first, leafNode->m_indecies.first);
    std::pair<int, int> break2 = std::make_pair(leafNode->m_indecies.first, leafNode->m_pNext->m_indecies.first);
    std::pair<int, int> bp;
    bool needReOrder = false;
    Node* pParent = leafNode->m_pParent;
    Node* pGparent = pParent;

    if (pParent->equals(break1))
    {
        bp = break2;
        needReOrder = false;
    }
    else
    {
        bp = break1;
        needReOrder = true;
    }

    while (pGparent != nullptr)
    {
        if (pGparent->equals(bp))
            break;
        pGparent = pGparent->m_pParent;
    };

    // the order remains node(prev->index, this->index), node(this->index, next->index).
    if (needReOrder)
    {
        res = std::make_pair(pGparent, pParent);
    }
    else
    {
        res = std::make_pair(pParent, pGparent);
    }
    return res;
}

void BeachLine::AddNode(uint32_t index, std::vector<Point2d>& points, double xValue, std::vector<HalfEdge*>& vHalfEdges)
{
    if (m_pRoot == nullptr)
    {
        m_pRoot = CreateNode(index, index, points, xValue);
    }
    else
    {
        AddNode(m_pRoot, index, points, xValue, vHalfEdges);
    }
}

Node* BeachLine::FindNode(Node* pNode, double xValue, double sweepLine)
{
    Node* pTmpNode = pNode;
    while (pTmpNode != nullptr)
    {
        if (pTmpNode->isLeaf())
        {
            break;
        }
        if (pTmpNode->findXofTheArc(sweepLine) >= xValue)
        {
            pTmpNode = pTmpNode->m_pLeft;
        }
        else
        {
            pTmpNode = pTmpNode->m_pRight;
        }
    }
    return pTmpNode;
}

// index : new node index.
// node : new node.
void BeachLine::AddNode(Node* pRootNode, uint32_t index, std::vector<Point2d>& points, double xValue, std::vector<HalfEdge*>& vHalfEdges)
{

    double sweepLine = points[index].y;

    // 1. find the prev arc base Point.
    Node* pNode = FindNode(pRootNode, xValue, sweepLine);
  
    assert(pNode != nullptr);
    if ((pNode)->m_pEvent != nullptr)
    {
        // Delete wrong alert.
        // we add both two kind of circle event in the basePoint event
        // like i->j->k, and k->j->i, so we only need one, then skip other.
        (pNode)->m_pEvent->m_eventType = EventType::Delete;
    }

    // 2. Make & insert the subTree to the Beachline Tree.
    Node* prevNode = pNode;
    Node* pParent = prevNode->m_pParent;
    uint32_t prevIndex = prevNode->m_indecies.first;
    Node* pRes = MakeSubTree(prevIndex, index, points, sweepLine, vHalfEdges);
    assert(pRes != nullptr);

    if (m_pRoot->isNakeRoot())
    {
        m_pRoot = pRes;
    }

    if (pRes != nullptr)
    {
        if ((pNode)->m_pPrev != nullptr)
            connect((pNode)->m_pPrev, m_pLeftLeafNow);
        if ((pNode)->m_pNext != nullptr)
            connect(m_pRightLeafNow, (pNode)->m_pNext);

        if (pParent != nullptr)
        {
            if(pParent->m_pRight == pNode)
            {
                pParent->m_pRight = pRes;
            }
            else
            {
                pParent->m_pLeft = pRes;
            }
        }
        pRes->m_pParent = pParent;
        (pNode) = pRes;
        // 3. Balance the Tree.
        while (pParent != nullptr)
        {
            Node* pResNode = BalanceTree(pParent);
            // update RootNode.
            pNode = pParent;
            pParent = pResNode->m_pParent;
        }
    }
}

void BeachLine::RemoveNode(Node** node, std::vector<Point2d>& points)
{
    deleteNode(node, points);
}

Node* BeachLine::deleteNode(Node** node, std::vector<Point2d>& points)
{
    // should be the leafNode.
    assert((*node)->isLeaf());

    // prev:jj
    // node:ii
    // next:kk
    Node* pNode = *node;
    Node* pParent = pNode->m_pParent;
    Node* pGrandParent = pParent->m_pParent;

    // ik
    std::pair<int, int> breakPoint1 = std::make_pair(pNode->m_indecies.first, pNode->m_pNext->m_indecies.first);

    // ji
    std::pair<int, int> breakPoint2 = std::make_pair(pNode->m_pPrev->m_indecies.first, pNode->m_indecies.first);

    // parent
    std::pair<int, int> parentIndex = std::make_pair(pParent->m_indecies.first, pParent->m_indecies.second);
    
    std::pair<int, int> otherParent;
    if (pParent->equals(breakPoint1))
    {
        otherParent = breakPoint2;
    }
    else
    {
        otherParent = breakPoint1;
    }

    // For example: 
    // 1.pGrandParent->m_pRight == pParent 
    // 2.pParent->m_pRight == pNode
    //  gParent                               gParent
    //  /     \             =>                /    \
    // node   pParent                       node    left
    //         /    \
    //       left   pNode
    //
    if (pGrandParent->m_pRight == pParent)
    {
        if (pParent->m_pRight == pNode)
        {
            pGrandParent->m_pRight = pParent->m_pLeft;
            pParent->m_pLeft->m_pParent = pGrandParent;
        }
        else
        {
            pGrandParent->m_pRight = pParent->m_pRight;
            pParent->m_pRight->m_pParent = pGrandParent;
        }
    }
    else
    {
        if (pParent->m_pRight == pNode)
        {
            pGrandParent->m_pLeft = pParent->m_pLeft;
            pParent->m_pLeft->m_pParent = pGrandParent;
        }
        else
        {
            pGrandParent->m_pLeft = pParent->m_pRight;
            pParent->m_pRight->m_pParent = pGrandParent;
        }
    }

    Node* pNewRoot = pGrandParent;
    while (pGrandParent != nullptr)
    {
        if (pGrandParent->equals(otherParent))
        {
            // Because the midle point is dispear, the prev and next will connect togther.
            uint32_t indexPre = pNode->m_pPrev->m_indecies.first;
            uint32_t indexNext = pNode->m_pNext->m_indecies.first;
            pGrandParent->m_indecies = std::make_pair(indexPre, indexNext);
            pGrandParent->m_pPoint1 = &points[indexPre];
            pGrandParent->m_pPoint2 = &points[indexNext];
        }
        pGrandParent = BalanceTree(pGrandParent);
        pNewRoot = pGrandParent;
        pGrandParent = pGrandParent->m_pParent;
    }
    connect(pNode->m_pPrev, pNode->m_pNext);
    m_pRoot = pNewRoot;
    return m_pRoot;
}

/*
        x1                   x2
          \                 /
           x2     ->       x1
           /                \
         x3                 x3
*/
void BeachLine::LeftRotate(Node** node)
{
    // 1. get the right node of this node.
    Node* nodeRight = (*node)->m_pRight;
    Node* parent = (*node)->m_pParent;

    // 2. get the left node of root right node.
    Node* toBeRootRight = nodeRight->m_pLeft;

    // 3. left node of root right node to be the new right node of root.
    (*node)->m_pRight = toBeRootRight;
    if (toBeRootRight != nullptr)
    {
        toBeRootRight->m_pParent = *node;
    }

    // 4. the new root node struct to be the left node of previous root right node.
    nodeRight->m_pLeft = *node;
    (*node)->m_pParent = nodeRight;

    // 5. update the top node.
    if (parent != nullptr)
    {
        if (parent->m_pLeft == *node)
        {
            parent->m_pLeft = nodeRight;
        }
        else
        {
            parent->m_pRight = nodeRight;
        }
    }
    nodeRight->m_pParent = parent;
    *node = nodeRight;

    if (parent == nullptr)
    {
        m_pRoot = *node;
    }
}

/*
     x1                   x2
    /                      \
   x2          ->          x1
     \                     /
      x3                  x3
*/
void BeachLine::RightRotate(Node** node)
{
    // 1. get the left node of root.
    Node* nodeLeft = (*node)->m_pLeft;
    Node* parent = (*node)->m_pParent;

    // 2. get the right node of root left node.
    Node* toBeRootLeft = nodeLeft->m_pRight;

    // 3. right node of root left node to be the new left node of root.
    (*node)->m_pLeft = toBeRootLeft;
    if (toBeRootLeft != nullptr)
    {
        toBeRootLeft->m_pParent = *node;
    }

    // 4. the new root node struct to be the right node of previous  root right node.
    nodeLeft->m_pRight = *node;
    (*node)->m_pParent = nodeLeft;

    // 5. update the top node.
    if (parent != nullptr)
    {
        if (parent->m_pLeft == *node)
        {
            *node = parent->m_pLeft;
            parent->m_pLeft = nodeLeft;
        }
        else
        {
            parent->m_pRight = nodeLeft;
        }
    }
    nodeLeft->m_pParent = parent;
    *node = nodeLeft;

    if (parent == nullptr)
    {
        m_pRoot = *node;
    }
}

int BeachLine::GetNodeHeight(Node* node)
{
    uint32_t height = 0;
    if (node != nullptr)
    {
        height = std::max(GetNodeHeight(node->m_pLeft), GetNodeHeight(node->m_pRight)) + 1;
    }
    return height;
};

int BeachLine::GetNodeRightHeight(Node* node)
{
    return node != nullptr ? GetNodeHeight(node->m_pRight) : 0;
}

int BeachLine::GetNodeLeftHeight(Node* node)
{
    return node != nullptr ? GetNodeHeight(node->m_pLeft) : 0;
}

Node* BeachLine::CreateNode(uint32_t prevIndex, uint32_t index, std::vector<Point2d>& points, double sweepline)
{
    Point2d* point1 = &points[prevIndex];
    Point2d* point2 = &points[index];

    Node* node = new Node(point1, point2, prevIndex, index, sweepline);
    nodeList.push_back(node);
    return node;
}

Node* BeachLine::BalanceTree(Node* node)
{
    if (node != nullptr)
    {
        if (GetNodeRightHeight(node) - GetNodeLeftHeight(node) > 1)
        {
            if (GetNodeLeftHeight(node->m_pRight) - GetNodeRightHeight(node->m_pRight) > 1)
            {
                RightRotate(&node->m_pRight);
            }
            LeftRotate(&node);
        }
        else if (GetNodeLeftHeight(node) - GetNodeRightHeight(node) > 1)
        {
            if (GetNodeRightHeight(node->m_pLeft) - GetNodeLeftHeight(node->m_pLeft) > 1)
            {
                LeftRotate(&node->m_pLeft);
            }
            RightRotate(&node);
        }
    }
    return node;
}