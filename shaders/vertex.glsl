 #version 330 core
layout (location = 0) in vec2 aPos;

uniform float TIME;

out vec2 fragPos;

void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	fragPos = aPos;
}
