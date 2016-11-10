#pragma once

#include "Material.h"
#include "Texture.h"

#include <glm.hpp>
#include <GL/glew.h>

#include <string>

struct FontGlyph {
    glm::vec2 beginUV;
    glm::vec2 endUV;

    FontGlyph()
    {

    }

    FontGlyph(const glm::vec2 &beginUV, const glm::vec2 &endUV)
        : beginUV(beginUV), endUV(endUV)
    {
    }
};

class Font
{
public:
    Font(const std::string &fileName, const GLint size);
    ~Font();
    const FontGlyph *getGlyph(const GLint charIndex) const;
    const Texture *getTexture() const;
    bool isOk() const;

    static bool initialize();
    static void destroy();
private:
    const Texture *texture;
    const FontGlyph *glyphs;
    const GLint size;
};
