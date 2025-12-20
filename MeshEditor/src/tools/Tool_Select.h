// File: MeshEditor/src/tools/Tool_Select.h
#pragma once

#include "Tool.h"
#include "scene/Scene.h"
#include "scene/Selection.h"
#include "ui/ViewportWidget.h"

namespace mesh
{
class Tool_Select : public Tool
{
public:
    Tool_Select(Scene& scene, Selection& selection, ViewportWidget& viewport);

    ToolType Type() const override { return ToolType::Select; }
    QString Name() const override { return QStringLiteral("Select"); }

private:
    Scene& scene_;
    Selection& selection_;
    ViewportWidget& viewport_;
};
}
