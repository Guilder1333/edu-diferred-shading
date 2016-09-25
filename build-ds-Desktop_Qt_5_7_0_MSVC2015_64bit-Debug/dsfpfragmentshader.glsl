#version 450

uniform sampler2D tex;
uniform sampler2D shadowTex;
uniform vec4 color;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;
in vec4 shadowPos;

out vec4 outPosition;
out vec4 outNormal;
out vec4 outColor;

void main() {
    vec3 projCoords = shadowPos.xyz / shadowPos.w;
    float closestDepth = texture2D(shadowTex, projCoords.xy).r;
    float attenuation = projCoords.z - 0.005f > closestDepth  ? 0.5f : 1.0f;
    vec4 c = (texture2D(tex, fragTexCoord) + color) * attenuation;
    outColor = c;
    outNormal = vec4(fragNormal, 1.0f);
    outPosition = vec4(fragPos, gl_FragCoord.z);

}
