#pragma once
#include <vector>
#include <set>

enum class SelectionMode {
    Object,
    Vertex,
    Edge,
    Face
};

class Selection {
public:
    SelectionMode mode = SelectionMode::Object;
    std::set<int> selectedVertices;
    std::set<int> selectedEdges;
    std::set<int> selectedFaces;
    
    void clear();
    void setMode(SelectionMode newMode);
    bool hasSelection() const;
};
