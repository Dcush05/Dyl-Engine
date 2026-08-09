#version 330 core
layout (location = 0) in vec3 position;
layout (location = 2) in vec4 color;
layout (location = 3) in vec3 tex_coords;
layout (location = 5) in vec3 size;



uniform mat4 projection;
uniform bool is_rounded;


out vec4 color_vs;


void main()
{
	gl_Position = projection * vec4(position, 1.0);

	if(is_rounded)
	{
		color_vs = color;
	}
}






