#pragma once

#include <glm.hpp>
#include "Texture.h"

class Material
{
public:
    Material(const class Texture *texture, const glm::vec4 diffuseColor);
    Material(const class Texture *texture);
    Material(const glm::vec4 diffuseColor);

    const Texture *getTexture() const;
    const glm::vec4 &getDiffuseColor() const;
private:
    const class Texture *texture;
    glm::vec4 diffuseColor;
};
