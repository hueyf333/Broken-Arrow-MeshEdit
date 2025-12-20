// File: MeshEditor/src/engine/Core.h
#pragma once

#include <memory>
#include <string>
#include <vector>

namespace MeshEditor
{
    using String = std::string;
    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T, typename... Args>
    Ref<T> MakeRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}
