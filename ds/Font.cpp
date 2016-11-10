#include "Font.h"

#include "Logger.h"

#include <ft2build.h>
#include <freetype.h>
#include <ftglyph.h>
#include <ftimage.h>

FT_Library library = nullptr;

Font::Font(const std::string &fileName, const GLint size)
    : size(size), glyphs(nullptr), texture(nullptr)
{
    FT_Face face;
    if (FT_New_Face(library, fileName.c_str(), 0, &face)) {
        Logger::log(std::string("Failed to load face '") + fileName + "'.");
        return;
    }

    FT_Set_Char_Size(face, size << 6, size << 6, 96, 96);

    const GLint sheetSize = size << 4;
    GLint width = 2;
    GLint height = 2;
    while (width < sheetSize) {
        width <<= 1;
    }
    while(height < sheetSize) {
        height <<= 1;
    }

    GLbyte *bytes = new GLbyte[width * height] {0};
    FontGlyph* glyphs = new FontGlyph[128];
    int left = 0;
    int top = 0;
    for(int i = 0; i < 128; i++) {
        if (FT_Load_Glyph(face, FT_Get_Char_Index(face, i), FT_LOAD_DEFAULT)) {
            Logger::log(std::string("Failed to load glyph '") + std::to_string(i) + "'.");
        }

        FT_Glyph glyph;
        if(FT_Get_Glyph(face->glyph, &glyph)) {
            Logger::log(std::string("Failed to get glyph."));
        }

        FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, 0, 1);
        const FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
        const FT_Bitmap& bitmap = bitmapGlyph->bitmap;

        glm::vec2 begin = glm::vec2(left, top);
        for(GLuint y = 0; y < bitmap.rows; y++) {
            for(GLuint x = 0; x < bitmap.width; x++) {
                bytes[left + x + (top + y) * size] = bitmap.buffer[x + y * bitmap.width];
            }
        }
        left += bitmap.width;
        const glm::vec2 end = glm::vec2(left, top + bitmap.rows);
        glyphs[i].beginUV = begin;
        glyphs[i].endUV = end;
        if ((i & 15) == 15) {
            top += size;
        }
    }

    this->glyphs = glyphs;
    this->texture = new Texture(width, height, GL_ALPHA8, GL_ALPHA, GL_UNSIGNED_BYTE, bytes);

    FT_Done_Face(face);
}

const FontGlyph *Font::getGlyph(const GLint charIndex) const
{
    if (this->glyphs == nullptr) {
        return nullptr;
    }
    return &this->glyphs[charIndex];
}

const Texture *Font::getTexture() const
{
    return this->texture;
}

bool Font::isOk() const
{
    return this->texture != nullptr && this->glyphs != nullptr;
}

bool Font::initialize()
{
    if (FT_Init_FreeType(&library)) {
        Logger::log(std::string("Failed to initialize FreeType library."));
        return false;
    }
    return true;
}

void Font::destroy()
{
    FT_Done_FreeType(library);
}
