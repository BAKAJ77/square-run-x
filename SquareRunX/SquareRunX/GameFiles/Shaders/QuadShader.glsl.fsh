#version 330 core

struct Material
{
    sampler2D DiffuseMap;
    vec4 DiffuseColor;
    bool Emissive, UseTextures;

    float BrightnessThreshold, OpacityMultiplier;
};

in VSH_OUT
{
    vec2 UVCoords;
} FshIn;

uniform Material Mat;
out vec4 FragColor;

void main()
{
    vec4 FinalColor;
    if(Mat.UseTextures)
    {
        FinalColor = texture(Mat.DiffuseMap, FshIn.UVCoords);
        FinalColor.rgb *= Mat.BrightnessThreshold;
        FinalColor.a *= Mat.OpacityMultiplier;
    }
    else
        FinalColor = Mat.DiffuseColor;

    if(FinalColor.a == 0.0f)
        discard;

    FragColor = FinalColor;
}
