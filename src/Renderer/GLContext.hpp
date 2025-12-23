#pragma once
#include <GLFW/glfw3.h>
#include <string>

class GLContext {
public:
    GLContext(int width, int height, const std::string& title);
    ~GLContext();
    
    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    
    GLFWwindow* getWindow() const { return m_window; }
    void getFramebufferSize(int& width, int& height) const;
    
private:
    GLFWwindow* m_window;
    
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);
};
