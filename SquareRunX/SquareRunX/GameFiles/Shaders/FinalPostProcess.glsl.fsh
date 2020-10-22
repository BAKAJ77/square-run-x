#version 330 core

in VSH_OUT
{
    vec2 UVCoords;
} FshIn;

uniform sampler2DMS FramebufferTexture;
uniform vec2 ResolutionSize;

uniform int NumSamplesMS;
uniform float Gamma;
uniform float Opacity;

out vec4 FragColor;

// Helper functions
vec3 GetMultisampledTexel(sampler2DMS TextureMS, int NumSamples, ivec2 Coord)
{
    vec3 TexelColor = vec3(0.0f);
    for(int i = 0; i < NumSamples; i++)
        TexelColor += texelFetch(TextureMS, ivec2(Coord), i).rgb;

    return TexelColor / float(NumSamples);
}

vec3 GetGammaCorrectedColor(vec3 Color, float GammaValue)
{
    return pow(Color, vec3(1.0f / GammaValue));
}

// The main shader function
void main()
{
    vec3 SampledTexelColor = GetMultisampledTexel(FramebufferTexture, NumSamplesMS, ivec2(FshIn.UVCoords * ResolutionSize));
    FragColor = vec4(GetGammaCorrectedColor(SampledTexelColor, Gamma), Opacity);
}
