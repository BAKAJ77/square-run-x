#version 330 core

struct Material
{
    sampler2D DiffuseMap;
    bool Emissive;
};

in VSH_OUT
{
    vec2 UVCoords;
} FshIn;

uniform Material Mat;
out vec4 FragColor;

void main()
{
    vec4 TextureColor = texture(Mat.DiffuseMap, FshIn.UVCoords);
    FragColor = TextureColor;
}
