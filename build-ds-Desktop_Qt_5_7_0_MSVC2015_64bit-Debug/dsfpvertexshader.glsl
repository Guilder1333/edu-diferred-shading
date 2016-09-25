#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 vertTexCoord;

uniform mat4 modelView;
uniform mat4 proj;
uniform mat4 modelMatrix;
uniform mat4 shadowMvp;
uniform mat3 normalMatrix;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragPos;
out vec4 shadowPos;
out gl_PerVertex {
    vec4 gl_Position;
};

void main(void) {
    vec4 v = vec4(position, 1.0f);
    fragPos = vec3(modelMatrix * v);
    fragNormal = normalize(normalMatrix * normal);
    fragTexCoord = vertTexCoord;
    shadowPos = shadowMvp * v;
    gl_Position = proj * modelView * v;
}
