#version 330 core
out vec4 outputColor;

in VertexOutput
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 positionLightSpace;
} fragmentInput;

struct Light
{
    vec3 direction;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

uniform Light light;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 viewPosition;

/*
float chebyshevUpperBound(vec4 fragPosLightSpace)
{
    float p = 0.0;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // We retrieve the two moments previously stored (depth and depth * depth)
    vec2 moments = texture(shadowMap, projCoords.xy).rg;

    // Surface is fully lit. as the current fragment is before the light occluder
    if (projCoords.z <= moments.x)
        p = 1.0;

    // The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
    // How likely this pixel is to be lit (p_max)
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.000001);

    float d = projCoords.z - moments.x;
    float pMax = variance / (variance + d * d);

    return max(p, pMax);
}
*/
void main()
{          
    vec3 color = vec3(texture(diffuseTexture, fragmentInput.uv));
    
    // ambient
    vec3 ambient = light.ambient * color;
    
    // diffuse 
    vec3 norm = normalize(fragmentInput.normal);
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, fragmentInput.uv));
    
    // specular
    vec3 viewDir = normalize(viewPosition - fragmentInput.position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    vec3 specular = light.specular * spec * vec3(1.0);

    //float visibility = chebyshevUpperBound(fragmentInput.positionLightSpace);
    float visibility = 1.0;
    vec3 lighting = (ambient + visibility * (diffuse + specular)) * color;    
    
    outputColor = vec4(pow(lighting, vec3(1.0 / 2.2)), 1.0);
}