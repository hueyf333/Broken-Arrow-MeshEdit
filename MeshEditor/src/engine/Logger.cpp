// File: MeshEditor/src/engine/Logger.cpp
#include "Logger.h"

#include <QDateTime>

namespace mesh
{
Logger& Logger::Get()
{
    static Logger instance;
    return instance;
}

void Logger::Attach(QTextEdit* widget)
{
    widget_ = widget;
}

void Logger::Info(const QString& message)
{
    if (widget_)
    {
        widget_->append(QStringLiteral("[Info] %1").arg(message));
    }
}

void Logger::Warn(const QString& message)
{
    if (widget_)
    {
        widget_->append(QStringLiteral("[Warn] %1").arg(message));
    }
}

void Logger::Error(const QString& message)
{
    if (widget_)
    {
        widget_->append(QStringLiteral("[Error] %1").arg(message));
    }
}
}
