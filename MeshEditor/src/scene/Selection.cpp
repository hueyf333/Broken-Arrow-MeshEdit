// File: MeshEditor/src/scene/Selection.cpp
#include "scene/Selection.h"

namespace MeshEditor
{
    void Selection::SetSelected(uint64_t id)
    {
        mSelected = id;
    }

    uint64_t Selection::Selected() const
    {
        return mSelected;
    }

    bool Selection::HasSelection() const
    {
        return mSelected != 0;
    }

    void Selection::Clear()
    {
        mSelected = 0;
    }
}
