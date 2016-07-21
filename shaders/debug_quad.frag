#version 330 core
out vec4 color;
in vec2 uv;

uniform sampler2D depthMap;

void main()
{             
    //float depthValue = texture(depthMap, uv).r;
    color = vec4(vec3(texture(depthMap, uv).r), 1.0);
    //color = vec4(1, 0, 1, 1);
}  