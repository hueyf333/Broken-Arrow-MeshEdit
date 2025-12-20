// File: MeshEditor/src/scene/Selection.h
#pragma once

#include <cstdint>

namespace MeshEditor
{
    class Selection
    {
    public:
        void SetSelected(uint64_t id);
        uint64_t Selected() const;
        bool HasSelection() const;
        void Clear();

    private:
        uint64_t mSelected = 0;
    };
}
