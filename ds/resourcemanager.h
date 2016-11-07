#pragma once

#include "Object.h"
#include "Light.h"

#include <glm.hpp>

#include <string>

class ResourceManager;

class ResourceManager {
public:
    virtual ~ResourceManager();
    const Material *loadMaterial(const std::string &texture, const glm::vec4 diffuseColor);
    Object *load3dsObject(const std::string &fileName);
    Mesh *load3dsMesh(const std::string &fileName, const std::string &meshName);
    Light *createLight();

    static void setInstnace(ResourceManager *manager);
    static ResourceManager* instance();

private:
    std::vector<Material*> materials;
    std::vector<Renderable *> objects;
};
