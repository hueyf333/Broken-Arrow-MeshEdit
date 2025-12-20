// File: MeshEditor/src/tools/Tool_Extrude.h
#pragma once

#include "Tool.h"

namespace MeshEditor
{
class Tool_Extrude : public Tool
{
public:
    std::string GetName() const override { return "Extrude"; }
};
}
