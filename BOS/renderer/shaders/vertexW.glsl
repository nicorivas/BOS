#version 330 core

layout(location = 0) in vec3 vpos;
layout(location = 1) in vec3 vnor;
layout(location = 2) in vec3 vcol;

uniform mat4 MVP;

out vec3 normal_pass;
out vec3 color_pass;

void main()
{
  gl_Position = MVP * vec4(vpos, 1.0f);
  normal_pass = vnor;
  color_pass = vcol;
}
