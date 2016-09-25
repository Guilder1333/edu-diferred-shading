#pragma once
#include "Mesh.h"
#include "Renderable.h"
#include <vector>
#include <glm.hpp>
#include <GL/glew.h>

class Object : public Renderable
{
public:
	Object(const char *fileName);
	~Object();

	virtual void setMatrix(glm::mat4 &);
    virtual glm::mat4 getMatrix();
    virtual glm::vec4 diffuseColor();
	void display();
    std::vector<Mesh*>& getMeshes();
private:
	glm::mat4 mx;
	std::vector<Mesh *> meshes;
};

