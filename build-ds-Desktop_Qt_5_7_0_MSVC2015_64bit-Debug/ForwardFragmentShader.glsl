#version 450

uniform sampler2D tex;
uniform sampler2D shadowTex;
uniform vec4 color;
uniform vec3 lightPos;
uniform int fragState;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;
in vec4 shadowPos;

out vec4 frag_color;
void main() {
    if (fragState == 0) {
        vec3 s = normalize(lightPos - fragPos);
        vec3 r = reflect(-s, fragNormal);
        vec3 v = normalize(-fragPos);
        float spec = max(dot(v, r), 0.0f);
        float diff = max(dot(fragNormal, s), 0.0f);
        vec4 c = (texture2D(tex, fragTexCoord) + color);
        vec3 diffColor = diff * c.xyz;
        vec3 specColor = pow(spec, 3) * vec3(c);
        vec3 ambientColor = vec3(0.1f, 0.1f, 0.1f);

        vec3 projCoords = shadowPos.xyz;// / shadowPos.w;
        projCoords = projCoords * 0.5f + 0.5f;
        float closestDepth = texture2D(shadowTex, projCoords.xy).r;
        float attenuation = projCoords.z - 0.005f > closestDepth  ? 0.5f : 1.0f;
        //diff = texture2D(shadowTex, shadowPos.xy).r;//*/
        frag_color = vec4((diffColor + /*0.5f * specColor +*/ ambientColor) * attenuation, 1.0f);
    } else {
        frag_color = vec4(vec3(texture2D(shadowTex, fragTexCoord).r), 1.0f);
    }
}
