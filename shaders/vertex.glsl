#version 330 core
layout (location = 0) in vec2 aPos;

uniform float TIME;

out vec2 fragPos;

out vec3 pos;
out vec3 up;
out vec3 right;
out vec3 front;
out vec3 sun;

void main()
{
	pos		= vec3(0.0, -2.0, 0.0);
	up		= vec3(0.0, 0.0, 1.0);
	right	= vec3(1.0, 0.0, 0.0);

	front = cross(up, right);

	sun = normalize(vec3(cos(TIME), sin(TIME), cos(TIME*0.56783346) * 0.1));

	gl_Position = vec4(aPos, 0.0, 1.0);
	fragPos = aPos;
}
