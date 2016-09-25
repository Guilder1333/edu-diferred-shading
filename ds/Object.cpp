#include "Object.h"

Object::Object(const char *fileName) : mx(glm::mat4())
{
	Lib3dsFile *file = lib3ds_file_load(fileName);
	if (file == nullptr) {
		return;
	}
	Lib3dsMesh *m = nullptr;
	for (Lib3dsMesh * m = file->meshes; m != nullptr; m = m->next)
	{
		meshes.push_back(new Mesh(file, m, this));
	}

	//delete []vertices;
	lib3ds_file_free(file);
}

Object::~Object()
{
    for(auto it = meshes.cbegin(); it != meshes.cend(); it++) {
        delete (Mesh*)*it;
    }
}

void Object::setMatrix(glm::mat4 &mx)
{
	this->mx = mx;
}

glm::mat4 Object::getMatrix()
{
	return mx;
}

glm::vec4 Object::diffuseColor()
{
	return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Object::display()
{
    for(auto it = meshes.cbegin(); it != meshes.cend(); it++) {
        ((Mesh*)*it)->display();
    }
}

std::vector<Mesh*>& Object::getMeshes()
{
    return meshes;
}
