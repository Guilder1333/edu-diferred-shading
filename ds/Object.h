#pragma once
#include "Mesh.h"
#include <vector>

class Object : public Renderable
{
public:
    Object(const std::vector<const Mesh *> *meshes);
    virtual ~Object();

    void display() const;
    const std::vector<const Mesh *> *getMeshes() const;
private:
    const std::vector<const Mesh *> *meshes;
};

