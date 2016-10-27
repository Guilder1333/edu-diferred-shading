#include "Object.h"
#include "lib3ds/file.h"
#include "lib3ds/mesh.h"

Object::Object(const std::vector<const Mesh *> *meshes)
    : meshes(meshes)
{

}

Object::~Object()
{
    delete this->meshes;
}

void Object::display() const
{
    for(auto it = this->meshes->cbegin(); it != this->meshes->cend(); it++) {
        ((const Mesh*)*it)->display();
    }
}

const std::vector<const Mesh *> *Object::getMeshes() const
{
    return this->meshes;
}
