#include "Texture.h"

#include "Logger.h"

#include <FreeImage.h>
#include <GL/glew.h>

Texture::Texture(const std::string &fileName)
    : fileName(fileName), texture(0)
{
    std::string fileNameWithExt;
    size_t index = fileName.find_last_of('.');
    if (index == std::string::npos) {
        fileNameWithExt = fileName + std::string(".png");
    } else {
        fileNameWithExt = fileName;
    }
    const char *fname = fileNameWithExt.c_str();

    //pointer to the image, once loaded
    FIBITMAP *dib(0);
    //OpenGL's image ID to map to

    //check the file signature and deduce its format
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(fname, 0);
    //if still unknown, try to guess the file format from the file extension
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(fname);
    //if still unkown, return failure
    if (fif == FIF_UNKNOWN) {
        Logger::log(std::string("Failed to get image type from file '") + fileName + std::string("', or file not exists."));
        return;
    }

    //check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(fif))
        dib = FreeImage_Load(fif, fname);
    //if the image failed to load, return failure
    if (!dib) {
        Logger::log(std::string("Failed to load image from file '") + fileName + std::string("'."));
        return;
    }

    //retrieve the image data
    const BYTE* bits = FreeImage_GetBits(dib);
    //get the image width and height
    const GLuint width = FreeImage_GetWidth(dib);
    const GLuint height = FreeImage_GetHeight(dib);
    //if this somehow one of these failed (they shouldn't), return failure
    if ((bits == 0) || (width == 0) || (height == 0)) {
        Logger::log(std::string("Failed to load image from file '") + fileName + std::string("'."));
        return;
    }

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
           format = GL_BGRA;
           break;
        default:
            Logger::log(std::string("Unsupported image color format '") + fileName + std::string("'."));
            return;
    }

    GLuint gl_texID;
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

Texture::Texture(const GLsizei width, const GLsizei height, const GLenum iternalFormat, const GLenum format, const GLenum componentFormat)
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
    if (this->texture != 0) {
        glDeleteTextures(1, &this->texture);
    }
}

void Texture::setParameters(const GLint magFilter, const GLint minFilter, const GLint wrapS, const GLint wrapT)
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

GLuint Texture::getId() const
{
    return this->texture;
}

void Texture::use() const
{
    if (this->texture != 0) {
        glBindTexture(GL_TEXTURE_2D, this->texture);
    }
}

void Texture::unbind(const GLint index)
{
    glActiveTexture(index);
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool Texture::isOk() const
{
    return this->texture != 0;
}
