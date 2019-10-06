#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 uv;

uniform float WIDTH;
uniform float HEIGHT;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	uv = aPos * vec2(1.0, WIDTH/HEIGHT);
}
