#version 330 core

layout(location = 0) in vec3 vpos;
layout(location = 2) in vec3 vcolor;

uniform mat4 MVP;

out vec3 normal_pass;
out vec3 color_pass;

void main()
{
  gl_Position = MVP * vec4(vpos, 1.0f);
  normal_pass = vpos;
  color_pass = vec3(1.0, 0, 0);
}
