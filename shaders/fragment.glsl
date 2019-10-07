#version 330 core
#define PI 3.141592659
#define APPROX_0 0.001
#define MAX_STEPS 128
#define MAX_DIST 256.0

out vec4 FragColor;

in vec2 fragPos;

uniform float TIME;

uniform vec2 WIN_SIZE;

uniform float WIDTH;
uniform float HEIGHT;

in vec3 sun;

vec2 hash2(vec2 p)
{
	vec3 a = fract(p.xyx*vec3(123.65, 219.53, 235.532));
	a += dot(a, a + 54.23);
	return fract(vec2(a.x*a.y, a.y*a.z));
}

float voronoi(vec2 p)
{
	vec2 ip = floor(p);
	vec2 dp = p - ip;
	float min_dist = 10.0;
	for(int i = -1; i != 2; ++i)
	{
		for(int j = -1; j != 2; ++j)
		{
			vec2 lp = vec2(float(i), float(j));
			float dist = length(hash2(ip+lp)-(dp-lp-vec2(0.5)));

			if(min_dist > dist)
			{
				min_dist = dist;
			}
		}
	}

	return min_dist;
}

// snoise from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
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

vec2 deform(vec2 p, float f)
{
	return vec2(p.x + snoise(p.xyx)*f, p.y + snoise(p.yxy)*f);
}

vec2 deform(vec2 p, float f, int o)
{
	return vec2(p.x + octave(p.xyx, o)*f, p.y + octave(p.yxy, o)*f);
}

float getH(vec2 p)
{
	return voronoi(deform(p*5.0, sin(TIME)*0.25, 3) * vec2(1.0, 0.5));
}

vec3 getNormal(vec2 p) {
	float h0 = getH(p);
	vec3 v1 = vec3(APPROX_0, 0.0, getH(p + vec2(APPROX_0, 0.0)) - h0);
	vec3 v2 = vec3(0.0, APPROX_0, getH(p + vec2(0.0, APPROX_0)) - h0);

    return normalize(cross(v1, v2));
}

vec3 getColorAt(vec2 uv)
{
	vec3 color = vec3(0.0, 0.3, 1.0);

	float diff = dot(sun, getNormal(uv)) * 0.5 + 0.5;

	diff = pow(diff, 1/2.2);

	color *= diff;

	return color;
}

void main()
{
	vec2 uv = fragPos * vec2(WIN_SIZE.x/WIN_SIZE.y, 1.0);

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

	FragColor = vec4(color, 1.0);
}