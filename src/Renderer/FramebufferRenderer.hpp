#pragma once
#include <glad/glad.h>

class FramebufferRenderer {
public:
    FramebufferRenderer();
    ~FramebufferRenderer();
    
    void resize(int width, int height);
    void bind();
    void unbind();
    
    GLuint getTexture() const { return m_texture; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
private:
    GLuint m_fbo;
    GLuint m_texture;
    GLuint m_rbo;
    int m_width;
    int m_height;
    
    void create();
    void destroy();
};
