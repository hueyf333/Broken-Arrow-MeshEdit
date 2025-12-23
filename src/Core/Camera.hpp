#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMode {
    Perspective,
    Orthographic
};

class Camera {
public:
    Camera();
    
    void setAspect(float aspect) { m_aspect = aspect; updateProjection(); }
    void setMode(CameraMode mode) { m_mode = mode; updateProjection(); }
    void toggleMode() { m_mode = (m_mode == CameraMode::Perspective) ? CameraMode::Orthographic : CameraMode::Perspective; updateProjection(); }
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const { return m_projection; }
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getTarget() const { return m_target; }
    
    // Orbit controls
    void orbit(float deltaYaw, float deltaPitch);
    void pan(float deltaX, float deltaY);
    void zoom(float delta);
    
    // Frame selection
    void frameTarget(const glm::vec3& target, float radius);
    
    // Ray casting for picking
    glm::vec3 getRayDirection(float ndcX, float ndcY) const;
    
private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    
    float m_distance;
    float m_yaw;
    float m_pitch;
    
    float m_fov;
    float m_aspect;
    float m_near;
    float m_far;
    float m_orthoSize;
    
    CameraMode m_mode;
    glm::mat4 m_projection;
    
    void updatePosition();
    void updateProjection();
};
