// File: MeshEditor/src/ui/Dock_Outliner.cpp
#include "ui/Dock_Outliner.h"

#include <QVBoxLayout>

namespace MeshEditor
{
    Dock_Outliner::Dock_Outliner(App& app, QWidget* parent)
        : QDockWidget("Outliner", parent), mApp(app)
    {
        QWidget* container = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(container);
        mTree = new QTreeWidget(container);
        mTree->setHeaderLabels({"Scene"});
        layout->addWidget(mTree);
        container->setLayout(layout);
        setWidget(container);

        connect(mTree, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int)
        {
            uint64_t id = item->data(0, Qt::UserRole).toULongLong();
            mApp.GetSelection().SetSelected(id);
        });
    }

    void Dock_Outliner::Refresh()
    {
        mTree->clear();
        for (const auto& entity : mApp.GetScene().Entities())
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(mTree);
            item->setText(0, QString::fromStdString(entity->Name()));
            item->setData(0, Qt::UserRole, QVariant::fromValue<qulonglong>(entity->Id()));
            mTree->addTopLevelItem(item);
        }
    }
}
