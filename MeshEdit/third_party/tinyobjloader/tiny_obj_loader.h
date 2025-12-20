#pragma once

#include <string>
#include <vector>

namespace tinyobj {
struct attrib_t {
    std::vector<float> vertices;
};

struct shape_t {
    std::string name;
};

struct material_t {
    std::string name;
};

inline bool LoadObj(attrib_t* attrib,
                    std::vector<shape_t>* shapes,
                    std::vector<material_t>* materials,
                    std::string* warn,
                    std::string* err,
                    const char* filename) {
    (void)attrib;
    (void)shapes;
    (void)materials;
    if (warn) {
        *warn = "tinyobjloader stub: parsing disabled.";
    }
    if (err) {
        *err = std::string("tinyobjloader stub: unable to load ") + filename;
    }
    return false;
}
}  // namespace tinyobj
