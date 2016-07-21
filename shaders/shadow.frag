#version 330 core

in vec4 _position;

out vec4 color;

void main()
{             
    //gl_FragDepth = gl_FragCoord.z;

    float depth = _position.z / _position.w;
    depth = depth * 0.5 + 0.5;

    float dx = dFdx(depth);
    float dy = dFdy(depth);

    float moment1 = depth;
    float moment2 = depth * depth - 0.25 * (dx * dx + dy * dy);

    color = vec4(moment1, moment2, 0.0, 1.0);
}  