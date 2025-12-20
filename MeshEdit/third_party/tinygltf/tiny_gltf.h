#pragma once

#include <string>

namespace tinygltf {
struct Model {
};

class TinyGLTF {
public:
    bool LoadASCIIFromFile(Model* model, std::string* err, std::string* warn, const std::string& filename) {
        (void)model;
        if (warn) {
            *warn = "tinygltf stub: parsing disabled.";
        }
        if (err) {
            *err = "tinygltf stub: unable to load " + filename;
        }
        return false;
    }
};
}  // namespace tinygltf
