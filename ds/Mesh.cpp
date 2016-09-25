#include "Mesh.h"
#include <lib3ds\file.h>
#include <lib3ds\mesh.h>
#include <lib3ds\material.h>
#include <FreeImage.h>
#include <GL/glew.h>
#include <gtc/type_ptr.hpp>

GLuint loadTexture(const char* filename, GLenum image_format, GLint internal_format) {
	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//pointer to the image, once loaded
	FIBITMAP *dib(0);
	//pointer to the image data
	BYTE* bits(0);
	//image width and height
	unsigned int width(0), height(0);
	//OpenGL's image ID to map to
	GLuint gl_texID;

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(filename, 0);
	//if still unknown, try to guess the file format from the file extension
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filename);
	//if still unkown, return failure
	if (fif == FIF_UNKNOWN)
		return false;

	//check that the plugin has reading capabilities and load the file
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filename);
	//if the image failed to load, return failure
	if (!dib)
		return false;

	//retrieve the image data
	bits = FreeImage_GetBits(dib);
	//get the image width and height
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);
	//if this somehow one of these failed (they shouldn't), return failure
	if ((bits == 0) || (width == 0) || (height == 0))
		return false;

	//generate an OpenGL texture ID for this texture
	glGenTextures(1, &gl_texID);
	//bind to the new texture ID
	glBindTexture(GL_TEXTURE_2D, gl_texID);
	//store the texture data for OpenGL use
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, image_format, GL_UNSIGNED_BYTE, bits);

	//Free FreeImage's copy of the data
	FreeImage_Unload(dib);

	//return success
	return gl_texID;
}

Mesh::Mesh(const char *fileName, const char *name) : parent(nullptr)
{
	Lib3dsFile *file = lib3ds_file_load(fileName);
	if (file == nullptr) {
		return;
	}
	Lib3dsMesh *m = nullptr;
	if (name != nullptr) {
		m = lib3ds_file_mesh_by_name(file, name);
	}
	if (m == nullptr) {
		m = file->meshes;
	}

	initialize(file, m);
	
	//delete []vertices;
	lib3ds_file_free(file);
}

Mesh::Mesh(Lib3dsFile * file, const Lib3dsMesh * mesh, Renderable *parent) : parent(parent)
{
	this->initialize(file, mesh);
}


Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &buffer);
    glDeleteTextures(1, &texture);
    glDeleteSamplers(1, &sampler);
}

void Mesh::setMatrix(glm::mat4 &mx)
{
	this->mx = mx;
}

glm::mat4 Mesh::getMatrix()
{
	if (parent == nullptr) {
		return mx;
	}
    return parent->getMatrix() * mx;
}

glm::vec4 Mesh::diffuseColor()
{
	return color;
}

void Mesh::display()
{
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBindSampler(0, sampler);
	glDrawArrays(GL_TRIANGLES, 0, this->points);
}

void Mesh::initialize(Lib3dsFile* file, const Lib3dsMesh *m)
{
	int stride = 6;
	bool hasTexCoords = m->points == m->texels;
	if (hasTexCoords) {
		stride += 2;
	}

    mx = glm::mat4();
	this->points = m->faces * 3;
	GLfloat *vertices = new GLfloat[this->points * stride]; // nine per face.
	char *material = nullptr;
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

		if (material == nullptr) {
			material = face.material;
		}
	}

	if (material != nullptr) {
		Lib3dsMaterial *mat = lib3ds_file_material_by_name(file, material);
		if (hasTexCoords) {
			texture = loadTexture("WoodTexture.png", GL_BGRA, GL_RGBA8);

			glGenSamplers(1, &sampler);
			glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			color = glm::vec4(.0f, .0f, .0f, .0f);
		}
		else {
			color = glm::make_vec4(mat->diffuse);
		}
	}

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->points * stride, vertices, GL_STATIC_DRAW);

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
}
