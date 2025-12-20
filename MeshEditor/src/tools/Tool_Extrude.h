// File: MeshEditor/src/tools/Tool_Extrude.h
#pragma once

#include "Tool.h"

namespace mesh
{
class Tool_Extrude : public Tool
{
public:
    ToolType Type() const override { return ToolType::Extrude; }
    QString Name() const override { return QStringLiteral("Extrude"); }
};
}
