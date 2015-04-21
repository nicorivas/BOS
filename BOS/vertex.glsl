#version 330 core

layout(location = 0) in vec3 vpos;

layout(location = 2) in vec3 objcolor;
layout(location = 3) in vec3 objpos;
layout(location = 4) in float objradius;
uniform mat4 MVP;

out vec3 normal_pass;
out vec3 color_pass;

void main()
{
  gl_Position = MVP * vec4(vpos * objradius + objpos, 1.0f);
  normal_pass = vpos;
  color_pass = objcolor;
}
