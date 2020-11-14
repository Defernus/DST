#version 330 core
precision highp float;

out vec4 FragColor;

in vec2 fragPos;

uniform float TIME;

uniform vec2 WIN_SIZE;

uniform float WIDTH;
uniform float HEIGHT;

in vec3 sun;

const float TIME_FACTOR = .1;
const float PI = 3.14159265;
const float MAX_DISTANCE = 100.;
const int MAX_RENDER_ITERATIONS = 1000;
const int MAX_FRACTAL_ITERATIONS = 250;
const float EPSILON = 0.0001;
const float DISTANCE_FACTOR = 1.;//for fractals
const vec3 BACKGROUND_COLOR = vec3(0., 0.7, 1.);
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
	float time = TIME*TIME_FACTOR;
    vec3 dir = normalize(cam.front + uv.x * cam.right + uv.y * cam.top);
    float rayLength = 0.;

    FractalData minDistData = FractalData(1., 1., 1.);
    
    for(int i = 0; i != MAX_RENDER_ITERATIONS && rayLength < MAX_DISTANCE; ++i) {
        vec3 point = cam.pos + dir * rayLength;
        FractalData data = getMandelbulb(point, sin(time/2.)*10.+ 12.);
        rayLength += data.dist;
        if(data.dist < minDistData.dist) {
            minDistData = data;
        }
        if(data.dist < EPSILON * scale) {
            break;
        }
    }
    vec3 color = vec3(
        sin( log(minDistData.pathDist) * sin(time*.3457) )*.5+.5,
        sin( log(minDistData.pathDist) * sin(time*.22475) )*.5+.5,
        sin( log(minDistData.pathDist) * sin(time*.364257) )*.5+.5
    );
    return mix(color*vec3(pow(minDistData.factor, 11.)), BACKGROUND_COLOR, minDistData.dist);
}

void main()
{
	float time = TIME*TIME_FACTOR;
	float scale = 1./(sin(time/11.33)*1.+2.);
	vec2 uv = fragPos * vec2(1.0, WIN_SIZE.y/WIN_SIZE.x) * scale;

    vec3 camPos = rotate(vec3(0., 3., 0.), vec3(0., 0., 1.), .345+time/1.);
    //camPos = rotate(camPos, , 1.124 +time/7.);

    Camera cam = getNewCamera(
    	camPos,
    	vec3(0.),
    	vec3(0., 0., 1.)
    );

	FragColor = vec4(getColor(uv, cam, scale), 1.);
}