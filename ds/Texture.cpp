#include "Texture.h"
#include <GL/glew.h>
#include <FreeImage.h>

Texture::Texture(const std::string &fileName)
    : fileName(fileName), texture(0)
{
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

    const char *fname = fileName.c_str();
    //check the file signature and deduce its format
    fif = FreeImage_GetFileType(fname, 0);
    //if still unknown, try to guess the file format from the file extension
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(fname);
    //if still unkown, return failure
    if (fif == FIF_UNKNOWN)
        return;

    //check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, fname);
    //if the image failed to load, return failure
    if (!dib)
        return;

    //retrieve the image data
    bits = FreeImage_GetBits(dib);
    //get the image width and height
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    //if this somehow one of these failed (they shouldn't), return failure
    if ((bits == 0) || (width == 0) || (height == 0))
        return;

    const int bpp = FreeImage_GetBPP(dib);
    GLint internalFormat;
    GLint format;
    switch(FreeImage_GetColorType(dib)) {
        case FIC_RGB:
            internalFormat = GL_RGB8;
            format = GL_RGB;
            break;
        case FIC_RGBALPHA:
           internalFormat = GL_RGBA8;
           format = GL_RGBA;
           break;
        default:
            return;
    }

    //generate an OpenGL texture ID for this texture
    glGenTextures(1, &gl_texID);
    //bind to the new texture ID
    glBindTexture(GL_TEXTURE_2D, gl_texID);
    //store the texture data for OpenGL use
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, bits);

    //Free FreeImage's copy of the data
    FreeImage_Unload(dib);

    //return success
    this->texture = gl_texID;
}

Texture::Texture(const int width, const int height, const int iternalFormat, const int format, const int componentFormat)
{
    GLuint gl_texID;
    glGenTextures(1, &gl_texID);
    this->texture = gl_texID;
    this->fileName = std::string("generated_") + std::to_string(gl_texID);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, iternalFormat, width, height, 0, format, componentFormat, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &this->texture);
}

void Texture::setParameters(const int magFilter, const int minFilter, const int wrapS, const int wrapT) const
{
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

const std::string &Texture::getFileName() const
{
    return this->fileName;
}

unsigned int Texture::getId() const
{
    return this->texture;
}

void Texture::use() const
{
    if (this->texture != 0) {
        glBindTexture(GL_TEXTURE_2D, this->texture);
    }
}

bool Texture::isOk() const
{
    return this->texture != 0;
}
