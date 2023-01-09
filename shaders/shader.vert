#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 uv_out;

layout(set = 0, binding = 0) uniform GlobalUbo{
    mat4 projectionViewMatrix;
    vec3 directionLight;
} ubo;

layout(push_constant) uniform Push{
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

const float AMBIENT = 0.05;

void main(){
//    gl_Position = vec4(mat3(push.transformation) * position + vec3(push.offset, 0.0), 1.0);
    gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

//    vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * normal);
    //optimize
//    mat3 normalMatrix = transpose(inverse(mat3(push.modelMatrix)));
    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

    //only works in certain conditions
    float lightIntensity = max(dot(normalWorldSpace, ubo.directionLight), AMBIENT);
    uv_out = uv;

    fragColor = lightIntensity * color;
}