#version 330
in vec2 uv;

uniform sampler2D textureSource;

out vec4 color;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 textureOffset = 1.0 / textureSize(textureSource, 0); // gets size of single texel
    vec3 result = texture(textureSource, uv).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(textureSource, uv + vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(textureSource, uv - vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(textureSource, uv + vec2(0.0, textureOffset.y * i)).rgb * weight[i];
            result += texture(textureSource, uv - vec2(0.0, textureOffset.y * i)).rgb * weight[i];
        }
    }
    color = vec4(result, 1.0);
}