#version 330 core
#define PI 3.141592659
#define APPROX_0 0.001
#define MAX_STEPS 128
#define MAX_DIST 256

out vec4 FragColor;

in vec2 fragPos;

uniform vec2 WIN_SIZE;

uniform float WIDTH;
uniform float HEIGHT;

in vec3 pos;
in vec3 up;
in vec3 right;
in vec3 front;

in vec3 sun;

float sphereDist(float r, vec3 sphere_pos, vec3 p)
{
	return length(p - sphere_pos) - r;
}

float sceneDist(vec3 p)
{
	return sphereDist(1.0, vec3(0.0), p);
}

float rayMarch(vec3 r_pos, vec3 r_dir) {
	r_dir = normalize(r_dir);
	float path_length=0.0;
    
    for(int i=0; i<MAX_STEPS; i++) {
    	vec3 p = r_pos + r_dir*path_length;
        float dist = sceneDist(p);
        path_length += dist;
        if(path_length>MAX_DIST || dist<APPROX_0) break;
    }
	
    return path_length;
}

vec3 getNormal(vec3 p) {
	float dist = sceneDist(p);
    vec3 n = dist - vec3(
        sceneDist(p - vec3(APPROX_0, 0.0, 0.0)),
        sceneDist(p - vec3(0.0, APPROX_0, 0.0)),
        sceneDist(p - vec3(0.0, 0.0, APPROX_0)));
    
    return normalize(n);
}
vec3 getColorAt(vec2 uv)
{
	vec3 ray = front+right*uv.x+up*uv.y;
	float dist = rayMarch(pos, ray);

	vec3 color = vec3(0.0);

	if(dist < MAX_DIST)
	{
		ray *= dist;
		vec3 norm = getNormal(ray);
		float diff = dot(norm, sun);
		color = vec3(diff/dist);
	}



	return color;
}

void main()
{
	int samples = 2;

	vec3 color = vec3(0.0);

	for(int  i = 0; i != samples; ++i)
	{
		for(int j = 0; j != samples; ++j)
		{
			color += getColorAt(fragPos * vec2(WIN_SIZE.x/WIN_SIZE.y, 1.0) + vec2(float(i)/float(samples), float(j)/float(samples))/WIN_SIZE.yy);
		}
	}

	color /= float(samples * samples);

	FragColor = vec4(color, 1.0);
}