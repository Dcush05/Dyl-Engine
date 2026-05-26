#version 330 core

layout(location = 0) in vec3 aPos;
//layout (location = 1) in vec4 aColor;
//layout (location = 2) in vec2 afset;
layout (location = 2) in vec2 tex_coords;
layout(location = 6) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;

out vec2 o_tex_coords;


void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
	o_tex_coords = tex_coords;
}

