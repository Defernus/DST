#version 330 core
precision highp float;

out vec4 FragColor;

in vec2 fragPos;

uniform float TIME;

uniform vec2 WIN_SIZE;
uniform vec2 MOUSE;
uniform vec2 SCROLL;

uniform float WIDTH;
uniform float HEIGHT;

const float PI = 3.14159265;
const float MAX_DISTANCE = 10.;
const int MAX_RENDER_ITERATIONS = 500;
const int MAX_FRACTAL_ITERATIONS = 100;
const float EPSILON = 0.0001;
const float DISTANCE_FACTOR = 1.;//for fractals
const vec3 BACKGROUND_COLOR = vec3(0., 0., 0.);
const vec3 lightDir = normalize(vec3(0.7, 0.9, -1.));

/** utils **/

vec4 mult(vec4 q1, vec4 q2) { 
  return vec4 (
  	(q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y),
  	(q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x),
    (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w),
    (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z)
  );
}

vec3 rotate(vec3 point, vec3 axe, float angle) {
    float sinA = sin(angle/2.0);
    float cosA = cos(angle/2.0);
    return mult(
        mult(
        	vec4(sinA * axe, cosA),
    		vec4(point.xyz, 0.)
    	),
        vec4(-sinA * axe, cosA)
    ).xyz;
}

/** ===== **/


/** camera data **/

struct Camera {
    vec3 pos;
    vec3 top;
    vec3 right;
    vec3 front;
};

Camera getNewCamera(vec3 pos, vec3 lookAt, vec3 top) {
    vec3 front = normalize(lookAt - pos);
    vec3 right = normalize(cross(front, top));
    return Camera(
        pos,
        normalize(cross(right, front)),
        right,
        front
    );
}

/** =========== **/


struct FractalData {
    float dist;
    float factor;
    float pathDist;
};

// distance estimator from http://celarek.at/wp/wp-content/uploads/2014/05/realTimeFractalsReport.pdf
FractalData getMandelbulb( vec3 pos, float Power ) {
    vec3 z = pos;
    float dr = 2.;
    float r;
    int i = 0;
    float pathDist = 0;
    for(; i != MAX_FRACTAL_ITERATIONS; ++i) {
        r = length(z);
        if(r > 2.) break;
        float theta = acos(z.z/r);
        float phi = atan(z.y, z.x );
        dr = pow(r, Power - 1.) * Power * dr + 1.;
        float zr = pow(r, Power);
        theta = theta * Power;
        phi = phi * Power;
        vec3 prevZ = z;
        z = zr * vec3(
            sin(theta)*cos(phi),
            sin(phi)*sin(theta),
            cos(theta)
        );
        z += pos;
        pathDist += distance(z, prevZ);
    }
    return FractalData(
        .5*log(r)*r / dr,
        1.-float(i)/float(MAX_FRACTAL_ITERATIONS),
    	pathDist
    );
}

vec3 getColor(vec2 uv, Camera cam, float scale) {
    vec3 dir = normalize(cam.front + uv.x * cam.right + uv.y * cam.top);
    float rayLength = 0.;

    FractalData minDistData = FractalData(1., 1., 1.);
    
    for(int i = 0; i != MAX_RENDER_ITERATIONS && rayLength < MAX_DISTANCE; ++i) {
        vec3 point = cam.pos + dir * rayLength;
        FractalData data = getMandelbulb(point, cos(TIME/10.)*5.+ 10.);
        rayLength += data.dist;
        if(data.dist < minDistData.dist) {
            minDistData = data;
        }
        if(data.dist < EPSILON * scale) {
            break;
        }
    }
    vec3 color = vec3(
        sin( log(log(minDistData.pathDist)) * (TIME*.01)    )*.5+.5,
        sin( log(log(minDistData.pathDist)) * (TIME*.01)*.6 )*.5+.5,
        sin( log(log(minDistData.pathDist)) * (TIME*.01)*.3 )*.5+.5
    );
    return mix(color*pow(minDistData.factor, 32.)*4., BACKGROUND_COLOR, minDistData.dist);
}

void main()
{
    float scroll = SCROLL.y*.1 +1.;
    scroll = pow(scroll, scroll);
	float scale = 1./(scroll+.1);
	vec2 uv = fragPos * vec2(1.0, WIN_SIZE.y/WIN_SIZE.x) * scale;
    vec2 rot = (MOUSE - WIN_SIZE/2.)/WIN_SIZE * PI*2.;

    vec3 camPos = rotate(vec3(0., 0., -2.), vec3(0., 1., 0.), rot.x);
    camPos = rotate(camPos, rotate(vec3(1., 0., 0.), vec3(0., 1., 0.), rot.x), rot.y);

    Camera cam = getNewCamera(
    	camPos,
    	vec3(0.),
    	vec3(0., 1., 0.)
    );

	FragColor = vec4(getColor(uv, cam, scale), 1.);
}