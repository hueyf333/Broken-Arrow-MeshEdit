#include "Core/Selection.hpp"

void Selection::clear() {
    selectedVertices.clear();
    selectedEdges.clear();
    selectedFaces.clear();
}

void Selection::setMode(SelectionMode newMode) {
    mode = newMode;
}

bool Selection::hasSelection() const {
    switch (mode) {
        case SelectionMode::Vertex: return !selectedVertices.empty();
        case SelectionMode::Edge: return !selectedEdges.empty();
        case SelectionMode::Face: return !selectedFaces.empty();
        default: return false;
    }
}
