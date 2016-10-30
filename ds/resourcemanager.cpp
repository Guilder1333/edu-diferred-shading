#include "ResourceManager.h"
#include "Material.h"

#include <lib3ds\file.h>
#include <lib3ds\mesh.h>
#include <lib3ds\material.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

static ResourceManager *manager;

const Lib3dsMaterial *findMaterial(const Lib3dsFile *file, const char *name) {
    for(Lib3dsMaterial *m = file->materials; m != nullptr; m = m->next) {
        if (strcmp(name, m->name) == 0) {
            return m;
        }
    }
    return nullptr;
}

Mesh *load3dsMesh(const Lib3dsFile *file, const Lib3dsMesh *m, ResourceManager *manager, const Renderable *parent)
{
    const GLboolean hasTexCoords = m->points == m->texels;
    GLint stride = 6;
    if (hasTexCoords) {
        stride += 2;
    }

    const GLint points = m->faces * 3;
    GLfloat *vertices = new GLfloat[points * stride]; // nine per face.
    GLchar *materialName = nullptr;
    for (GLuint i = 0, k = 0; i < m->faces; i++) {
        Lib3dsFace& face = m->faceL[i];
        vertices[k++] = m->pointL[face.points[0]].pos[0];
        vertices[k++] = m->pointL[face.points[0]].pos[1];
        vertices[k++] = m->pointL[face.points[0]].pos[2];
        vertices[k++] = face.normal[0];
        vertices[k++] = face.normal[1];
        vertices[k++] = face.normal[2];
        if (hasTexCoords) {
            vertices[k++] = m->texelL[face.points[0]][0];
            vertices[k++] = m->texelL[face.points[0]][1];
        }
        vertices[k++] = m->pointL[face.points[1]].pos[0];
        vertices[k++] = m->pointL[face.points[1]].pos[1];
        vertices[k++] = m->pointL[face.points[1]].pos[2];
        vertices[k++] = face.normal[0];
        vertices[k++] = face.normal[1];
        vertices[k++] = face.normal[2];
        if (hasTexCoords) {
            vertices[k++] = m->texelL[face.points[1]][0];
            vertices[k++] = m->texelL[face.points[1]][1];
        }
        vertices[k++] = m->pointL[face.points[2]].pos[0];
        vertices[k++] = m->pointL[face.points[2]].pos[1];
        vertices[k++] = m->pointL[face.points[2]].pos[2];
        vertices[k++] = face.normal[0];
        vertices[k++] = face.normal[1];
        vertices[k++] = face.normal[2];
        if (hasTexCoords) {
            vertices[k++] = m->texelL[face.points[2]][0];
            vertices[k++] = m->texelL[face.points[2]][1];
        }

        if (materialName == nullptr) {
            materialName = face.material;
        }
    }

    const Material *material(nullptr);
    if (materialName != nullptr) {
        std::string texture;
        glm::vec4 color;
        if (hasTexCoords)
        {
            texture = std::string("WoodTexture.png");
            color = glm::vec4(.0f, .0f, .0f, .0f);
        }
        else
        {
            const Lib3dsMaterial *mat = findMaterial(file, materialName);
            color = glm::make_vec4(mat->diffuse);
            texture = std::string();
        }
        material = manager->loadMaterial(texture, color);
    }

    GLuint buffer, vao;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * points * stride, vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if (hasTexCoords) {
        glEnableVertexAttribArray(2);
    }
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (const void *)(sizeof(GLfloat) * 3));
    if (hasTexCoords) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (const void *)(sizeof(GLfloat) * 6));
    }
    Mesh *mesh = new Mesh(vao, buffer, points, parent);
    mesh->setMaterial(material);
    return mesh;
}

ResourceManager::~ResourceManager()
{
    for(auto it = this->materials.begin(); it != this->materials.end(); it++) {
        delete (Material*)*it;
    }

    for(auto it = this->objects.begin(); it != this->objects.end(); it++) {
        delete (Renderable *)*it;
    }
}

const Material *ResourceManager::loadMaterial(const std::string &texture, const glm::vec4 diffuseColor)
{
    for(auto it = this->materials.cbegin(); it != this->materials.cend(); it++)
    {
        Material *mat = (Material *) *it;
        if (glm::all(glm::equal(mat->getDiffuseColor(), diffuseColor)) && texture.compare(mat->getTexture()->getFileName()) == 0)
        {
            return mat;
        }
    }

    Texture *tex;
    if (texture.empty()) {
        tex = nullptr;
    } else {
        tex = new Texture(texture);
    }
    Material *mat = new Material(tex, diffuseColor);
    this->materials.push_back(mat);
    return mat;
}

Object *ResourceManager::load3dsObject(const std::string &fileName)
{
    Lib3dsFile *file = lib3ds_file_load(fileName.c_str());
    if (file == nullptr) {
        return nullptr;
    }
    std::vector<const Mesh*> *meshes = new std::vector<const Mesh*>();
    Object *object = new Object(meshes);
    for (Lib3dsMesh * m = file->meshes; m != nullptr; m = m->next)
    {
        Mesh *mesh = ::load3dsMesh(file, m, this, object);
        meshes->push_back(mesh);
        this->objects.push_back(mesh);
    }

    //delete []vertices;
    lib3ds_file_free(file);

    this->objects.push_back(object);
    return object;
}

Mesh *ResourceManager::load3dsMesh(const std::string &fileName, const std::string &meshName)
{
    Lib3dsFile *file = lib3ds_file_load(fileName.c_str());
    if (file == nullptr) {
        return nullptr;
    }
    Lib3dsMesh *m = nullptr;
    if (!meshName.empty()) {
        m = lib3ds_file_mesh_by_name(file, meshName.c_str());
    }
    if (m == nullptr) {
        m = file->meshes;
    }

    Mesh *mesh = ::load3dsMesh(file, m, this, nullptr);

    //delete []vertices;
    lib3ds_file_free(file);

    this->objects.push_back(mesh);
    return mesh;
}

Light *ResourceManager::createLight()
{
    Light *light = new Light();
    this->objects.push_back(light);
    return light;
}

void ResourceManager::setInstnace(ResourceManager *manager)
{
    ::manager = manager;
}

ResourceManager *ResourceManager::instance()
{
    return ::manager;
}
