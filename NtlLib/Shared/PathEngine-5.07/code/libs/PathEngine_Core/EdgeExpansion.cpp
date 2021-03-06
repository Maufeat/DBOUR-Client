//**********************************************************************
//
// Copyright (c) 2006
// PathEngine
// Lyon, France
//
// All Rights Reserved
//
//**********************************************************************

#include "platform_common/Header.h"
#include "interface/EdgeExpansion.h"
#include "ExpandExternalEdges.h"
#include "ElementCollisionInfo.h"
#include "ElementInFaceLookup.h"
#include "ElementLookupCollisionTester.h"
#include "CombineEdgeExpansionPieces.h"
#include "interface/LineCollision.h"
#include "interface/QueryContext.h"
#include "libs/Mesh2D/interface/CircuitElement.h"
#include "libs/Mesh2D/interface/InternalPosition.h"
#include "libs/Mesh2D/interface/tMesh.h"
#include "libs/Mesh2D/interface/SaveCircuits_XML.h"
#include "libs/Mesh2D/interface/LoadCircuits_XML.h"
#include "libs/Mesh_Common/interface/MeshWithTrackedTransform.h"
#include "libs/Geometry/interface/tLine.h"
#include "libs/Geometry/interface/tPoint_Multiplication.h"
#include "libs/Geometry/interface/AxisFraction.h"
#include "common/interface/iXMLOutputStream.h"
#include "common/interface/iXMLInputStream.h"
#include "common/STL_Helpers.h"

cEdgeExpansion::cEdgeExpansion(
            cQueryContext& qc,
            cMeshWithTrackedTransform<tMesh>* mesh2D_NoInternalVertices,
            const cCollisionShape& expandShape
            )
{
    _edgeExpansion = new tMeshCutSequence;
    {
        std::vector<cCircuitElement*> pieces;
        if(mesh2D_NoInternalVertices)
        {
            ExpandExternalEdges(mesh2D_NoInternalVertices->mesh(), expandShape, pieces);
            cReversedMeshTranslator<tMesh> reverseTranslator(mesh2D_NoInternalVertices);
            long i;
            for(i = 0; i != SizeL(pieces); i += 2)
            {
                cMeshCut cut;
                cut.initFromFragment(pieces[i], pieces[i + 1]);
                cut.remap(reverseTranslator);
                cut.takeCircuit();
            }
        }
        else
        {
            ExpandExternalEdges(qc.refMesh(), expandShape, pieces);
        }
        CombineEdgeExpansionPieces(qc, pieces, qc.getNumberOfInternalFaces(), expandShape, *_edgeExpansion);
    }
    assertF(!CutSequenceContainsInlineCorner(*_edgeExpansion));
    NormaliseLinesCutSequence(*_edgeExpansion);

    _edgeExpansionElementCount = IndexElements(*_edgeExpansion, 0);
    _edgeExpansionLookup = ConstructAndBuildElementsInFaceLookup(*_edgeExpansion, qc.refMesh().faces());
    ShrinkToFit_Compound(*_edgeExpansionLookup);
    _edgeExpansionCollisionTester = new cElementLookupCollisionTester(*_edgeExpansionLookup, _edgeExpansionElementCount);
}

cEdgeExpansion::cEdgeExpansion(
        cQueryContext& qc,
        iXMLInputStream& is
        )
{
    is.openElement("edgeExpansion");
    _edgeExpansion = new tMeshCutSequence;
    LoadCutSequence_XML(qc.refMesh(), is, *_edgeExpansion);
    is.closeElement();
    _edgeExpansionElementCount = IndexElements(*_edgeExpansion, 0);
    _edgeExpansionLookup = ConstructAndBuildElementsInFaceLookup(*_edgeExpansion, qc.refMesh().faces());
    _edgeExpansionCollisionTester = new cElementLookupCollisionTester(*_edgeExpansionLookup, _edgeExpansionElementCount);
}

cEdgeExpansion::~cEdgeExpansion()
{
    delete _edgeExpansionLookup;
    delete _edgeExpansionCollisionTester;
}

void
cEdgeExpansion::save(iXMLOutputStream& os) const
{
    os.openElement("edgeExpansion");
    SaveCutSequence_XML(*_edgeExpansion, os);
    os.closeElement("edgeExpansion");
}

long
cEdgeExpansion::getElementCountRequirement() const
{
    return _edgeExpansionElementCount;
}


void
cEdgeExpansion::firstCollision(cQueryContext& qc, tFace startF, const tLine& line, bool& collided, cAxisFraction& distanceAlong, tLine& collidingLine) const
{
    assertD(line.start() != line.end());
    tFace endF_Ignored;
    cElementCollisionInfo info;
    cFlagVector& elementFlagVector = qc.lockElementFlagVector();
    FirstLineCollision(*_edgeExpansionLookup, elementFlagVector, startF, line, endF_Ignored, info);
    qc.releaseFlagVector();
    if(info.collisionOccurred())
    {
        cAxisFraction candidateDistance(info.n, info.d);
        if(!collided || candidateDistance < distanceAlong)
        {
            collided = true;
            distanceAlong = candidateDistance;
            info.getIntersectingLine(line, collidingLine);
        }
    }
}

void
cEdgeExpansion::firstCollision(
        cQueryContext& qc, tFace startF,
        const tLine& line, const cAxisFraction& startFraction, const cAxisFraction& endFraction,
        bool& collided, cAxisFraction& distanceAlong, tLine& collidingLine
        ) const
{
    assertD(line.start() != line.end());
    //... startFraction == endFraction case ought to be ok
    tFace endF_Ignored;
    cElementCollisionInfo info;
    cFlagVector& elementFlagVector = qc.lockElementFlagVector();
    FirstLineCollision(
        *_edgeExpansionLookup, elementFlagVector, startF,
        line, startFraction, endFraction,
        endF_Ignored, info
        );
    qc.releaseFlagVector();
    if(info.collisionOccurred())
    {
        cAxisFraction candidateDistance(info.n, info.d);
        if(!collided || candidateDistance < distanceAlong)
        {
            collided = true;
            distanceAlong = candidateDistance;
            info.getIntersectingLine(line, collidingLine);
        }
    }
}
