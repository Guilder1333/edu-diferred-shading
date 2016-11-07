#include "Renderable.h"
#include <gtc/matrix_transform.hpp>


Renderable::Renderable()
    : Renderable(nullptr)
{
}

Renderable::Renderable(const Renderable *parent)
    : position(glm::vec3(0.0f)), pitch(0.0f), yaw(0.0f), roll(0.0f),
      material(nullptr), parent(parent), scale(glm::vec3(1.0f))
{

}

Renderable::~Renderable()
{
}

void Renderable::setPosition(const glm::vec3 &position)
{
    this->position = position;
}

void Renderable::setPosition(const GLfloat x, const GLfloat y, const GLfloat z)
{
    this->position = glm::vec3(x, y, z);
}

void Renderable::setRotation(const GLfloat pitch, const GLfloat yaw, const GLfloat roll)
{
    this->pitch = pitch;
    this->yaw = yaw;
    this->roll = roll;
}

void Renderable::setScale(const GLfloat x, const GLfloat y, const GLfloat z)
{
    this->scale = glm::vec3(x, y, z);
}

void Renderable::setScale(const GLfloat scale)
{
    this->scale = glm::vec3(scale, scale, scale);
}

void Renderable::setScale(const glm::vec3 &scale)
{
    this->scale = scale;
}

const glm::vec3 &Renderable::getPosition() const
{
    return this->position;
}

glm::mat4 Renderable::getMatrix() const
{
    glm::mat4 mx = glm::translate(glm::mat4(), this->position);
    mx = glm::rotate(mx, this->pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    mx = glm::rotate(mx, this->yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    mx = glm::rotate(mx, this->roll, glm::vec3(0.0f, 0.0f, 1.0f));
    mx = glm::scale(mx, this->scale);
    if (this->parent == nullptr) {
        return mx;
    }
    return this->parent->getMatrix() * mx;
}

void Renderable::setMaterial(const Material *material)
{
    this->material = material;
}

const Material *Renderable::getMaterial() const
{
    return this->material;
}
