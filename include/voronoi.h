#pragma once
#include "BeachLine.h"
#include "DrawGraph.h"

static void InitEventQueue(const std::vector<Point2d>& vec);

class BeachLine;
class HalfEdge;
class DrawAgent;

class VoronoiBuilder
{
public:
    VoronoiBuilder()
        :
        m_pBeachLine(nullptr),
        m_drawAgent(DrawAgent())
    {
    };

    virtual ~VoronoiBuilder()
    {
        if (m_pBeachLine != nullptr)
        {
            delete m_pBeachLine;
            m_pBeachLine = nullptr;
        }

        for (size_t i = 0; i < m_Vertices.size(); i++)
        {
            Vertex* pVertex = m_Vertices[i];
            Destructor(pVertex)
        }

        for (size_t i = 0; i < m_VhalfEdges.size(); i++)
        {
            HalfEdge* pEdge = m_VhalfEdges[i];
            HalfEdge* pTwin = pEdge->pTwin;
            Destructor(pEdge)
                Destructor(pTwin)
        }
    };

    void BuildVoronoiGraph(std::vector<Point2d>& points);

    inline void SetBeachLine(BeachLine* pBeachLine)
    {
        m_pBeachLine = pBeachLine;
    }

    inline BeachLine* GetBeachLine()
    {
        return m_pBeachLine;
    }

    inline void DrawTheGraph(const std::vector<Point2d>& points, BoundingBox& bbox)
    {
        m_drawAgent.DrawTheGraph(points, bbox, m_VhalfEdges);
    }

private:
    std::vector<Vertex*> m_Vertices;
    std::vector<HalfEdge*> m_VhalfEdges;
    BeachLine* m_pBeachLine;
    DrawAgent m_drawAgent;
};