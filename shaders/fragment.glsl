#version 330 core
precision highp float;

out vec4 FragColor;

in vec2 uv;
in float scale;

uniform vec3 CAM_POSITION;
uniform vec3 CAM_FRONT;
uniform vec3 CAM_TOP;

uniform vec2 MOUSE;

uniform float TIME;

const float PI = 3.14159265;
const float MAX_DISTANCE = 16.;
const int MAX_RENDER_ITERATIONS = 500;
const int MAX_FRACTAL_ITERATIONS = 100;
const float EPSILON = 0.001;
const vec3 BACKGROUND_COLOR = vec3(0., 0., 0.);
const vec3 lightDir = normalize(vec3(0.7, 0.9, -1.));


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



struct Camera {
    vec3 pos;
    vec3 top;
    vec3 right;
    vec3 front;
};

Camera getNewCamera(vec3 pos, vec3 front, vec3 top) {
    vec3 right = normalize(cross(front, top));
    return Camera(
        pos,
        normalize(cross(right, front)),
        right,
        normalize(front)
    );
}



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
        FractalData data = getMandelbulb(point, cos(TIME/10.)*20.+ 22.);
        rayLength += data.dist;
        if(data.dist < minDistData.dist) {
            minDistData = data;
        }
        if(data.dist < EPSILON / pow(scale, scale)) {
            break;
        }
    }
    vec3 color = vec3(
        sin( log(log(minDistData.pathDist)/scale) * cos(TIME*2.017 + 1.) )*.5+.5,
        sin( log(log(minDistData.pathDist)/scale) * cos(TIME*2.039) )*.5+.5,
        sin( log(log(minDistData.pathDist)/scale) * cos(TIME*2.083) )*.5+.5
    );
    return mix(color*pow(minDistData.factor, 64./scale)*4., BACKGROUND_COLOR, minDistData.dist);
}

void main()
{
    Camera cam = getNewCamera(
    	CAM_POSITION,
    	CAM_FRONT,
    	CAM_TOP
    );

	FragColor = vec4(getColor(uv, cam, scale), 1.);
}