// File: MeshEditor/src/engine/Logger.h
#pragma once

#include <QString>
#include <QTextEdit>

namespace mesh
{
class Logger
{
public:
    static Logger& Get();
    void Attach(QTextEdit* widget);
    void Info(const QString& message);
    void Warn(const QString& message);
    void Error(const QString& message);

private:
    Logger() = default;
    QTextEdit* widget_ = nullptr;
};
}
