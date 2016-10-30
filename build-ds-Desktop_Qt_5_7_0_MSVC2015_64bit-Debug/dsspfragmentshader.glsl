#version 450

uniform sampler2D posTex;
uniform sampler2D normalTex;
uniform sampler2D colorTex;
uniform vec3 lightPos;

in vec2 uv;

out vec4 frag_color;

void main() {
    vec3 fragPos = texture2D(posTex, uv).xyz;
    vec4 n = texture2D(normalTex, uv);
    vec3 fragNormal = n.xyz;
    vec4 color = texture2D(colorTex, uv);
    vec3 s = normalize(lightPos - fragPos);
    vec3 r = reflect(-s, fragNormal);
    vec3 v = normalize(-fragPos);
    float spec = max(dot(v, r), 0.0f);
    float diff = max(dot(fragNormal, s), 1.f - n.w);
    vec3 specColor = pow(spec, 3) * color.xyz;
    vec3 ambientColor = vec3(0.1f, 0.1f, 0.1f) * n.w;
    frag_color = vec4((diff * color.xyz + /*0.5f * specColor +*/ ambientColor), color.w);
}
