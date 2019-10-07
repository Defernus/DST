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

// noise from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
vec4 permute(vec4 x)
{
	return mod(((x*34.0)+1.0)*x, 289.0);
}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v)
{ 
	const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
	const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

	vec3 i  = floor(v + dot(v, C.yyy) );
	vec3 x0 =   v - i + dot(i, C.xxx) ;

	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min( g.xyz, l.zxy );
	vec3 i2 = max( g.xyz, l.zxy );

	vec3 x1 = x0 - i1 + 1.0 * C.xxx;
	vec3 x2 = x0 - i2 + 2.0 * C.xxx;
	vec3 x3 = x0 - 1. + 3.0 * C.xxx;

	i = mod(i, 289.0 ); 
	vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

	float n_ = 1.0/7.0;
	vec3  ns = n_ * D.wyz - D.xzx;

	vec4 j = p - 49.0 * floor(p * ns.z *ns.z);

	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_ );

	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);

	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );

	vec4 s0 = floor(b0)*2.0 + 1.0;
	vec4 s1 = floor(b1)*2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
	vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

	vec3 p0 = vec3(a0.xy,h.x);
	vec3 p1 = vec3(a0.zw,h.y);
	vec3 p2 = vec3(a1.xy,h.z);
	vec3 p3 = vec3(a1.zw,h.w);

	vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
	m = m * m;
	return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), dot(p2,x2), dot(p3,x3) ) );
}

float octave(vec3 p, int o)
{
	float ret = 0;

	float scale = 1.0;

	for(int i = 0; i < o; ++i)
	{
		ret += snoise(p) * scale;
		p *= 2.0;
		scale *= 0.5;
	}

	return ret / (2.0-1.0/pow(2.0,(o-1.0)));
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
	FragColor = vec4(vec3(octave(vec3(uv, TIME), 8)*0.5 + 0.5 ), 1.0);
}