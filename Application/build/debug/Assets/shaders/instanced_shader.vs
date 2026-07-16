#version 450 core

layout(location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normals;
layout (location = 2) in vec2 a_tex_coords;
layout (location = 3) in vec4 a_color;
layout(location = 6) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec2 o_tex_coords;
out vec3 o_normals; 
out vec3 o_pos; 
out vec4 o_color;


out mat4 o_view;


void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(a_pos, 1.0);
	o_tex_coords = a_tex_coords;
	o_color = a_color;
	o_normals = a_normals;
	o_pos = a_pos;
}

