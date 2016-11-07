#include "Camera.h"

#include <gtc/matrix_transform.hpp>

Camera::Camera()
    : projection(glm::mat4())
{

}

Camera::~Camera()
{

}

void Camera::setPerspective(const GLfloat fovy, const GLfloat aspect, const GLfloat near, const GLfloat far)
{
    this->projection = glm::perspective(fovy, aspect, near, far);
}

void Camera::setOrtho(const GLfloat left, const GLfloat right, const GLfloat bottom, const GLfloat top, const GLfloat near, const GLfloat far)
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
