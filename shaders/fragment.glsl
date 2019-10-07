#version 330 core
#define PI 3.141592659
#define APPROX_0 0.001
#define MAX_STEPS 128
#define MAX_DIST 256

out vec4 FragColor;

in vec2 fragPos;

uniform float TIME;

uniform vec2 WIN_SIZE;

uniform float WIDTH;
uniform float HEIGHT;

in vec3 pos;
in vec3 up;
in vec3 right;
in vec3 front;

in vec3 sun;

float hash(vec3 p)
{
	p = round(p);
	return fract(sin(dot(vec3(p), vec3(1.2348, 1.2224, 1.2154)))*43758.5453123);
}

float mixFunc(float a, float b, float f)
{
	return a + (.5-cos(f*PI)*.5)*(b-a);
}

float noise(vec3 p)
{
	vec3 p0 = floor(p);
	vec3 dp = p - p0;
	vec3 p1 = p0 + sign(dp);

	float n000 = hash(ivec3(p0));
	float n001 = hash(ivec3(p0.xy, p1.z));
	float n010 = hash(ivec3(p0.x, p1.y, p0.z));
	float n011 = hash(ivec3(p0.x, p1.yz));
	float n100 = hash(ivec3(p1.x, p0.yz));
	float n101 = hash(ivec3(p1.x, p0.y, p1.z));
	float n110 = hash(ivec3(p1.xy, p0.z));
	float n111 = hash(ivec3(p1));

	float n00 = mixFunc(n000, n001, dp.z);
	float n01 = mixFunc(n010, n011, dp.z);
	float n10 = mixFunc(n100, n101, dp.z);
	float n11 = mixFunc(n110, n111, dp.z);

	float n0 = mixFunc(n00, n01, dp.y);
	float n1 = mixFunc(n10, n11, dp.y);

    return mixFunc (n0, n1, dp.x);
}

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
	vec2 uv = fragPos * vec2(WIN_SIZE.x/WIN_SIZE.y, 1.0);
	/*
	int samples = 2;

	vec3 color = vec3(0.0);

	for(int  i = 0; i != samples; ++i)
	{
		for(int j = 0; j != samples; ++j)
		{
			color += getColorAt(uv + vec2(float(i)/float(samples), float(j)/float(samples))/WIN_SIZE.yy);
		}
	}

	color /= float(samples * samples);
*/
	FragColor = vec4(vec3(noise(vec3(uv*10.0, TIME*1000.0))), 1.0);
}