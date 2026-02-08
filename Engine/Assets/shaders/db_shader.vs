#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec4 color;
layout (location = 3) in vec3 tex_coords;

uniform mat4 projection;
out vec4 vertexColor;

void main()
{
	gl_Position = projection * vec4(position, 1.0);
	vertexColor = color;	
}
