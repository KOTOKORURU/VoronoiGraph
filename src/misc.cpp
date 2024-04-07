#include "../include/misc.h"

std::pair<HalfEdge*, HalfEdge*> HalfEdge::makeHalfEdge(uint32_t leftIdx, uint32_t rightIdx)
{
    auto pEdge1 = new HalfEdge(leftIdx, rightIdx);
    auto pEdge2 = new HalfEdge(rightIdx, leftIdx);

    pEdge1->pTwin = pEdge2;
    pEdge2->pTwin = pEdge1;
    return std::make_pair(pEdge1, pEdge2);
}

void HalfEdge::connectHalfEdge(HalfEdge* pFirst, HalfEdge* pNext)
{
    pFirst->pNext = pNext;
    pNext->pPrev = pFirst;
}