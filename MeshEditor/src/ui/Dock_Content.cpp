// File: MeshEditor/src/ui/Dock_Content.cpp
#include "ui/Dock_Content.h"

#include <QVBoxLayout>

namespace MeshEditor
{
    Dock_Content::Dock_Content(App& app, QWidget* parent)
        : QDockWidget("Content Browser", parent), mApp(app)
    {
        QWidget* container = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(container);
        mList = new QListWidget(container);
        layout->addWidget(mList);
        container->setLayout(layout);
        setWidget(container);
    }

    void Dock_Content::Refresh()
    {
        mList->clear();
        for (const auto& asset : mApp.GetAssetDatabase().Assets())
        {
            mList->addItem(QString::fromStdString(asset));
        }
    }
}
