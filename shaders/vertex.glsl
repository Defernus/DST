 #version 330 core
layout (location = 0) in vec2 aPos;

out vec2 uv;
out float scale;

uniform vec2 SCROLL;
uniform vec2 WIN_SIZE;

void main()
{
    scale = SCROLL.y*.1 +1.;
	uv = aPos*vec2(1., WIN_SIZE.y/WIN_SIZE.x)*2.;

	gl_Position = vec4(aPos, 0.0, 1.0);
}
