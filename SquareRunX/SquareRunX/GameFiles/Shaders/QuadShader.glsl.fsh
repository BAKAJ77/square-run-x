#version 330 core

struct Material
{
    sampler2D Texture;
    bool Emissive;

    float BrightnessThreshold;
    float OpacityMultiplier;
};

in VSH_OUT
{
    vec2 UVCoords;
} FshIn;

uniform Material Mat;
out vec4 FragColor;

void main()
{
    vec4 TextureColor = texture(Mat.Texture, FshIn.UVCoords) * Mat.BrightnessThreshold;
    TextureColor.a *= Mat.OpacityMultiplier;

    FragColor = TextureColor;
}
