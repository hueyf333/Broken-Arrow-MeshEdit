#pragma once
#include <string>
#include <vector>

class ConsolePanel {
public:
    void render();
    void addMessage(const std::string& message);
    void clear();
    
private:
    std::vector<std::string> m_messages;
};
