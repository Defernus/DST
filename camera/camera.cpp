#include "camera.h"
#include <glm/gtx/transform.hpp>
#include <math.h>

Camera::Camera()
{
    pos = glm::vec3(0., 0., 0.);
    top = glm::vec3(0., 1., 0.);
    front = glm::vec3(0., 0., -1.);
}

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 top): pos(pos), front(glm::normalize(front))
{
    glm::vec3 right = normalize(cross(front, top));
    this->top = normalize(cross(right, front));
}

glm::vec3 Camera::getPos() const
{
    return pos;
}

glm::vec3 Camera::getFront() const
{
    return front;
}

glm::vec3 Camera::getTop() const
{
    return top;
}

glm::vec3 Camera::getRight() const
{
    return cross(front, top);
}

Camera &Camera::move(const glm::vec3 &val)
{
    pos += val;
    return *this;
}

Camera &Camera::setPos(glm::vec3 pos)
{
    this->pos = pos;
    return *this;
}

glm::vec4 mult(glm::vec4 q1, glm::vec4 q2)
{ 
  return glm::vec4 (
  	(q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y),
  	(q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x),
    (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w),
    (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z)
  );
}

glm::vec3 rotateAxeAngle(glm::vec3 point, glm::vec3 axe, float angle)
{
    float sinA = sin(angle/2.0);
    float cosA = cos(angle/2.0);
    glm::vec4 result = mult(
        mult(
        	glm::vec4(sinA * axe, cosA),
    		glm::vec4(point, 0.)
    	),
        glm::vec4(-sinA * axe, cosA)
    );
    return glm::vec3(result.x, result.y, result.z);
}

Camera &Camera::rotate(const glm::vec3 &axe, const float &angle)
{
    front = rotateAxeAngle(front, axe, angle);
    top = rotateAxeAngle(top, axe, angle);

    return *this;
}
