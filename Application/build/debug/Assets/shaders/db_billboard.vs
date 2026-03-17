#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 3) in vec3 tex_coords;
layout(location = 5) in vec3 size;


out vec2 v_tex_coords;
out vec3 vsize;



void main()
{
	gl_Position =  vec4(pos, 1.0);
	v_tex_coords = tex_coords.xy;
	vsize = size;
}
