#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normals;
layout(location = 6) in mat4 instanceMatrix;

uniform mat4 view;
uniform mat4 projection;
uniform float outline;




void main()
{

	float t = outline;
	vec3 scaled_pos =  a_pos + (a_normals * 0.05);
	gl_Position = projection * view * instanceMatrix * vec4(scaled_pos, 1.0);

}
