#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec4 color;
layout (location = 3) in vec3 tex_coords;

layout (location = 4) in float rotation;

uniform mat4 projection;
uniform mat4 model;
out vec4 vertexColor;

void main()
{
	vec3 pos;
	pos.x = (position.x * cos(rotation)) - (sin(rotation) * position.y);	
	pos.y = (position.x * sin(rotation)) - (cos(rotation) * position.y);	

	gl_Position = projection * model * vec4(pos, 1.0);
	vertexColor = color;	
}
