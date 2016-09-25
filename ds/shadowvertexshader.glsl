#version 450

layout(location = 0) in vec3 pos;

uniform mat4 mvp;

out gl_PerVertex {
    vec4 gl_Position;
};

void main(void)
{
    gl_Position = mvp * vec4(pos, 1.0f);
}
