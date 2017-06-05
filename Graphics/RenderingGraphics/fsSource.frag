#version 410

in vec4 vColour;

out vec4 FragColour;

uniform float Time;

void main()
{
	//vec4 colourChange;
	//colourChange.x = cos(Time);
	//colourChange.y += sin(Time);

	FragColour = vColour;
}