// File: MeshEditor/src/engine/Command.h
#pragma once

#include <memory>
#include <string>
#include <functional>

namespace mesh
{
class Command
{
public:
    virtual ~Command() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual std::string Name() const = 0;
};

using CommandPtr = std::unique_ptr<Command>;

class FunctionCommand final : public Command
{
public:
    FunctionCommand(std::string name, std::function<void()> execute, std::function<void()> undo);

    void Execute() override;
    void Undo() override;
    std::string Name() const override;

private:
    std::string name_;
    std::function<void()> execute_;
    std::function<void()> undo_;
};
}
