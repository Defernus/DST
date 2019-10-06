#version 330 core
out vec4 FragColor;

in vec2 uv;

uniform float TIME;

void main()
{
	vec3 color = 0.5 + 0.5*cos(TIME+uv.xyx+vec3(0,2,4));

	FragColor = vec4(color, 1.0);
}