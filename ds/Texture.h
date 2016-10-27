#pragma once
#include <string>

class Texture
{
public:
    Texture(const std::string &fileName);
    Texture(const int width, const int height, const int iternalFormat, const int format, const int componentFormat);
    ~Texture();

    void setParameters(const int magFilter, const int minFilter, const int wrapS, const int wrapT) const;

    bool isOk() const;
    const std::string &getFileName() const;
    unsigned int getId() const;
    void use() const;
private:
    unsigned int texture;
    std::string fileName;
};
