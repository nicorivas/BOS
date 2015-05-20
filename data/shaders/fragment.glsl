#version 330 core

in vec3 normal_pass;
in vec3 color_pass;

out vec3 color;

const vec3 lightOrigin = normalize(vec3(1, 0.2, 0));
const vec3 lightColor = vec3(1, 0.94, 0.86);
const vec3 lightAmbient = vec3(0.0949, 0.08, 0.076)*3;

void main() {
  color = color_pass;
  
  float cosLight = dot(normal_pass, lightOrigin);
  color *= (cosLight * lightColor) + lightAmbient;
  
}
