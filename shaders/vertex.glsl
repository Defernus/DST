 #version 330 core
layout (location = 0) in vec2 aPos;

uniform float TIME;

out vec2 fragPos;

out vec3 sun;

void main()
{
	//sun = normalize(vec3(cos(TIME), sin(TIME), 3.0));
	sun = normalize(vec3(0.125, 0.25, 1.0));

	gl_Position = vec4(aPos, 0.0, 1.0);
	fragPos = aPos;
}
