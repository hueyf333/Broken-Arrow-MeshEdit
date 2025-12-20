// File: MeshEditor/src/ui/Dock_Console.h
#pragma once

#include <QDockWidget>
#include <QTextEdit>

namespace MeshEditor
{
class Dock_Console : public QDockWidget
{
    Q_OBJECT

public:
    explicit Dock_Console(QWidget* parent = nullptr);
    void BindLogger();

public slots:
    void AppendLog(const QString& message);

private:
    QTextEdit* m_text;
};
}
