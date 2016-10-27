#include "Material.h"
#include "Texture.h"

Material::Material(const Texture *texture, const glm::vec4 diffuseColor)
    : texture(texture)
{
    this->diffuseColor = diffuseColor;
}

Material::Material(const Texture *texture)
    : Material(texture, glm::vec4(0))
{

}

Material::Material(const glm::vec4 diffuseColor)
    : Material(nullptr, diffuseColor)
{

}

const Texture *Material::getTexture() const
{
    return this->texture;
}

const glm::vec4 &Material::getDiffuseColor() const
{
    return this->diffuseColor;
}
