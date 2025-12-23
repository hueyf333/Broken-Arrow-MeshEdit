#pragma once
#include <string>
#include <glad/glad.h>

class Shader {
public:
    Shader(const std::string& vertPath, const std::string& fragPath);
    ~Shader();

    void use() const;
    GLuint getProgram() const { return m_program; }
    
    void setMat4(const char* name, const float* value) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;

private:
    GLuint m_program;
    
    static std::string readFile(const std::string& path);
    static GLuint compileShader(GLenum type, const std::string& source);
};
