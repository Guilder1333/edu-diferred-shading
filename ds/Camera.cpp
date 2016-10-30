#include "Camera.h"

#include <gtc/matrix_transform.hpp>

Camera::Camera()
    : projection(glm::mat4())
{

}

Camera::~Camera()
{

}

void Camera::setPerspective(const float fovy, const float aspect, const float near, const float far)
{
    this->projection = glm::perspective(fovy, aspect, near, far);
}

void Camera::setOrtho(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
    this->projection = glm::ortho(left, right, bottom, top, near, far);
}

const glm::mat4 Camera::getProjection() const
{
    return this->projection;
}

void Camera::display() const
{

}
