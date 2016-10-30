#include "Renderable.h"
#include <gtc/matrix_transform.hpp>


Renderable::Renderable()
    : Renderable(nullptr)
{
}

Renderable::Renderable(const Renderable *parent)
    : position(glm::vec3(0)), pitch(0), yaw(0), roll(0), material(nullptr), parent(parent)
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

const glm::vec3 &Renderable::getPosition() const
{
    return this->position;
}

glm::mat4 Renderable::getMatrix() const
{
    glm::mat4 mx = glm::translate(glm::mat4(), this->position);
    mx = glm::rotate(mx, pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    mx = glm::rotate(mx, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    mx = glm::rotate(mx, roll, glm::vec3(0.0f, 0.0f, 1.0f));
    if (this->parent == nullptr) {
        return mx;
    }
    return this->parent->getMatrix() * mx;
}

void Renderable::beforeDisplay() const
{

}

void Renderable::setMaterial(const Material *material)
{
    this->material = material;
}

const Material *Renderable::getMaterial() const
{
    return this->material;
}
