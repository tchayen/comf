#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out VertexOutput
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 positionLightSpace;
} vertexOutput;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexOutput.position = vec3(model * vec4(position, 1.0));
    vertexOutput.normal = transpose(inverse(mat3(model))) * normal;
    vertexOutput.uv = uv;
    vertexOutput.positionLightSpace = lightSpaceMatrix * vec4(vertexOutput.position, 1.0);
}