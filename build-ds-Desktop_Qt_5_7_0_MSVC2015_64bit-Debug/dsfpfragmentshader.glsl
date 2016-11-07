#version 450

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D shadowTex;
uniform vec4 color;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;
in vec4 shadowPos;

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outColor;

void main() {
    vec3 projCoords = shadowPos.xyz / shadowPos.w;
    float closestDepth = texture2D(shadowTex, projCoords.xy).r;
    float attenuation = projCoords.z - 0.005f > closestDepth  ? 0.5f : 1.0f;
    vec4 c;
    if (textureSize(texture1, 0).y > 0.f) {
        c = texture2D(texture1, fragTexCoord);
    } else {
        c = color;
    }
    if (textureSize(texture2, 0).y > 0.f) {
        c = mix(c, texture2D(texture2, fragTexCoord), 0.5);
    }
    outColor = c * attenuation;
    outNormal = vec4(fragNormal, 1.0f);
    outPosition = vec4(fragPos, gl_FragCoord.z);
}
