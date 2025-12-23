#pragma once
#include "Core/Mesh.hpp"
#include "Core/Selection.hpp"

namespace MeshOps {
    // Extrude selected faces
    void extrudeFaces(Mesh& mesh, const Selection& selection, float distance);
    
    // Bevel selected edges
    void bevelEdges(Mesh& mesh, const Selection& selection, float amount);
    
    // Subdivide mesh (Catmull-Clark style)
    void subdivide(Mesh& mesh);
    
    // Merge vertices by distance
    void mergeByDistance(Mesh& mesh, float threshold);
    
    // Delete selected elements
    void deleteVertices(Mesh& mesh, const Selection& selection);
    void deleteEdges(Mesh& mesh, const Selection& selection);
    void deleteFaces(Mesh& mesh, const Selection& selection);
    
    // Recalculate normals (flat vs smooth)
    void recalculateNormals(Mesh& mesh, bool smooth = true);
}
