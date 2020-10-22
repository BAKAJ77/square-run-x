#version 330 core
layout (location = 0) in vec2 VertexPos;
layout (location = 1) in vec2 UVCoords;
layout (location = 2) in mat4 InstanceModel;

out VSH_OUT
{
    vec2 UVCoords;
} VshOut;

uniform mat4 CameraMatrix;

void main()
{
    VshOut.UVCoords = UVCoords;
    gl_Position = CameraMatrix * InstanceModel * vec4(VertexPos, 0.0f, 1.0f);
}
