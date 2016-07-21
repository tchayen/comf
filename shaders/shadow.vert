#version 330 core
layout (location = 0) in vec3 position;

out vec4 _position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
    _position = gl_Position;
}  