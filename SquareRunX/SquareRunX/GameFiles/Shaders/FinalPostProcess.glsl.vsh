#version 330 core
layout (location = 0) in vec2 VertexCoord;
layout (location = 1) in vec2 UVCoords;

out VSH_OUT
{
    vec2 UVCoords;
} VshOut;

void main()
{
    VshOut.UVCoords = UVCoords;
    gl_Position = vec4(VertexCoord * 2.0f, 0.0f, 1.0f);
}
