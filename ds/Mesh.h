#pragma once
#include <glm.hpp>
#include <lib3ds\file.h>
#include <lib3ds\mesh.h>
#include "Renderable.h"

class Mesh : public Renderable
{
public:
	Mesh(const char *fileName, const char *name);
	Mesh(Lib3dsFile* file, const Lib3dsMesh *mesh, Renderable *parent);
	~Mesh();

	virtual void setMatrix(glm::mat4 &);
    virtual glm::mat4 getMatrix();
    virtual glm::vec4 diffuseColor();
    unsigned int getTexture() {
        return texture;
    }

	void display();
private:
	void initialize(Lib3dsFile* file, const Lib3dsMesh *mesh);

	Renderable *parent;

	glm::vec4 color;
	glm::mat4 mx;
	unsigned int buffer;
	unsigned int vao;
	int points;
	unsigned int texture;
	unsigned int sampler;
};

