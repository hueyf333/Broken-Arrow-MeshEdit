// File: MeshEditor/src/tools/Tool_Select.cpp
#include "Tool_Select.h"
#include "scene/Scene.h"
#include "scene/Selection.h"

namespace MeshEditor
{
void Tool_Select::OnMousePress(QMouseEvent* event, Scene* scene)
{
    if (!scene || !event)
        return;

    // TODO: implement ray cast picking from viewport.
    if (!scene->GetEntities().empty())
    {
        scene->GetSelection().SetPrimary(scene->GetEntities().front().GetId(), *scene);
    }
}
}
