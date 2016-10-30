#pragma once

class VertexShader;
class FragmentShader;

class ShaderProgram
{
public:
    ShaderProgram();
    virtual ~ShaderProgram();
    void assign(const VertexShader *shader);
    void assign(const FragmentShader *shader);
    virtual bool isOk() const;
    virtual void use() const;
    unsigned int getId() const;
    static void useProgram(const ShaderProgram *);
protected:
    virtual void link();
private:
    unsigned int program;
    const VertexShader *vertexShader;
    const FragmentShader *fragmentShader;
    bool linked;
};
