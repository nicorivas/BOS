#version 330 core

layout(location = 0) in vec3 vpos;

layout(location = 2) in vec3 objvelo;
layout(location = 3) in vec3 objpos;
layout(location = 4) in float objradius;
layout(location = 5) in int objspec;

uniform mat4 MVP;

out vec3 normal_pass;
out vec3 color_pass;

//#define SHEAR_CELL
#define SHEAR_BOX
//#define COLORMODE2
//#define CHUTE


const vec3 colorA = vec3(0.9, 0.9, 0.9);
const vec3 colorB = vec3(0.3254, 0.6353, 0.7450);
const vec3 colorC = vec3(0.8627, 1, 0.9921);

vec3 HSVToRGB(vec3 hsv) {
  float C = clamp(hsv[1],0,1) * clamp(hsv[2],0,1);
  float H_ = clamp(hsv[0],-1,360) / 60.0;
  float X = C * (1 - abs(mod(H_, 2) - 1));

  vec3 Rvec;
  if (H_ < 0) {
    Rvec = vec3(0,0,0);
  } else if (H_ < 1) {    // H 0..1
    Rvec = vec3(C,X,0);
  } else if (H_ < 2) {    // H 1..2
    Rvec = vec3(X,C,0);
  } else if (H_ < 3) {    // H 2..3
    Rvec = vec3(0,C,X);
  } else if (H_ < 4) {    // H 3..4
    Rvec = vec3(0,X,C);
  } else if (H_ < 5) {    // H 4..5
    Rvec = vec3(X,0,C);
  } else {                // H 5..6
    Rvec = vec3(C,0,X);
  }
  Rvec = Rvec + (clamp(hsv[2],0,1) - C) * vec3(1,1,1);

  return Rvec;
}

void main()
{
  vec3 obj2pos = vec3(objpos[0], objpos[1] * 2.5, objpos[2] * 1.2);
  gl_Position = MVP * vec4(vpos * (objradius * 1.2) + obj2pos, 1.0f);
  normal_pass = vpos;

  
  
#ifdef SHEAR_CELL
  const vec3 up = vec3(0,1,0);

  vec3 radVec = normalize(cross(objvelo - dot(objvelo,up) * up ,up));
  float radVelo = dot(objvelo,up);

#ifdef COLORMODE2
  radVelo = clamp(radVelo * 400, 0.0, 2.0) - 1.0;

  if (radVelo > 0) {
    color_pass = HSVToRGB(vec3(40,pow(radVelo,2),1));
  } else {
    color_pass = HSVToRGB(vec3(206,pow(-radVelo,2),1));
  }
#else
  color_pass = HSVToRGB( vec3(clamp(0 + 15000 *radVelo,0,100), 1, 1) );
#endif

#else
  vec3 color;
  if (objspec == 0) {
    color = colorA;
  } else if (objspec == 1) {
#ifdef CHUTE
    color = HSVToRGB(vec3(clamp(30*objvelo[1] + 123,40,206), max(objvelo[2],0) + clamp(objvelo[0]*10,0,0.2), 1));
#else
    color = colorB;
#endif
  } else {
    color = colorC;
  }
#ifdef CHUTE
  color_pass = color;
#else

#ifdef SHEAR_BOX
  color_pass = color * (0.8 + 0.05 * sqrt(length(objvelo))) + vec3(clamp(objradius * 2 - 1,0,1), 0, 0);
#else
  color_pass = color * (0.8 + 0.15 * sqrt(length(objvelo)));
#endif
#endif
#endif
}
