#version 410

in vec2 vTexCoord;
in vec3 vColour;

out vec4 FragColor;

uniform sampler2D diffuse;

void main()
{
	FragColor = texture(diffuse, vTexCoord) * vec4(vColour, 1);
}