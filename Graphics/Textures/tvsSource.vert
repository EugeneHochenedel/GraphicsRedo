#version 410

layout(location = 0) in vec4 Position;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Colour;

out vec3 vColour;
out vec2 vTexCoord;

uniform mat4 ProjectionView;

void main()
{
	vColour = Colour;
	vTexCoord = TexCoord;
	gl_Position = ProjectionView * Position;
}