#include "Text.h"

Text::Text(const Font *font, const std::string &text)
    : text(text), font(font)
{
    GLfloat *vertices = new GLfloat[text.size() * 6 * 4]; // 4 components per vertex (x,y,u,v), 6 vertices per glyph.
    GLfloat x = 0;
    GLint index = -1;
    for(auto it = text.cbegin(); it != text.cend(); it++)
    {
        const GLchar c = (GLchar)*it;
        const FontGlyph *glyph = font->getGlyph(c);
        const GLfloat height = glyph->endUV.y - glyph->beginUV.y;
        const GLfloat width = glyph->endUV.x - glyph->beginUV.x;
        // first triangle
        // left-top
        vertices[++index] = x;
        vertices[++index] = 0;
        vertices[++index] = glyph->beginUV.x;
        vertices[++index] = glyph->beginUV.y;
        // left-bottom
        vertices[++index] = x;
        vertices[++index] = height;
        vertices[++index] = glyph->beginUV.x;
        vertices[++index] = glyph->endUV.y;
        x += width;
        // right-bottom
        vertices[++index] = x;
        vertices[++index] = height;
        vertices[++index] = glyph->endUV.x;
        vertices[++index] = glyph->endUV.y;
        // second triangle
        // right-bottom
        vertices[++index] = x;
        vertices[++index] = height;
        vertices[++index] = glyph->endUV.x;
        vertices[++index] = glyph->endUV.y;
        // right-top
        vertices[++index] = x;
        vertices[++index] = 0;
        vertices[++index] = glyph->endUV.x;
        vertices[++index] = glyph->beginUV.y;
        // left-top
        vertices[++index] = x - width;
        vertices[++index] = 0;
        vertices[++index] = glyph->beginUV.x;
        vertices[++index] = glyph->beginUV.y;
    }

    GLuint buffer, vao;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * points * stride, vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (const void *)(sizeof(GLfloat) * 2));
    delete[]vertices;
}

void Text::display() const
{
    glBindVertexArray(this->vao);
    glDrawArrays(GL_TRIANGLES, 0, this->text.size() * 6);
}
