#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 top);
    glm::vec3 getPos() const;
    glm::vec3 getFront() const;
    glm::vec3 getTop() const;
    glm::vec3 getRight() const;
    
    Camera &move(const glm::vec3 &val);
    Camera &setPos(glm::vec3 pos);
    Camera &rotate(const glm::vec3 &axe, const float &angle);
private:
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 top;
};