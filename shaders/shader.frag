#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

layout(binding = 1) uniform sampler2D texSampler;

vec3 gammaCorrection(vec3 inColor, float gamma){
    return pow(inColor.rgb, vec3(1.0/gamma));
}

void main() {
    outColor = texture(texSampler, uv) * vec4(fragColor, 1.0);
//    outColor = texture(texSampler, uv);

    float gamma = 2.2;
    outColor.rgb = gammaCorrection(outColor.rgb, gamma);
}