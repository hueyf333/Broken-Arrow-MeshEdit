// File: MeshEditor/src/tools/Tool_Select.h
#pragma once

#include "tools/Tool.h"

namespace MeshEditor
{
    class Tool_Select : public Tool
    {
    public:
        QString Name() const override { return "Select"; }
    };
}
