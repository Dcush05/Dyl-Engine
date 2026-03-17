#version 330 core
layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform vec3 camera_pos;
uniform mat4 projection;
uniform mat4 view;

in vec3 vsize[];
//in vec2 v_tex_coords[];

out vec2 g_tex_coords;
//testing without errors
//mat4 projection;
//mat4 view;
//vec3 camera_pos;

void main()
{
	mat4 vp = projection * view;
	vec3 g_pos = gl_in[0].gl_Position.xyz;
	vec3 camera_to_point = normalize(g_pos - camera_pos);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, camera_to_point);
	//float w = vsize[0].x;

	//float w = vsize[0].y;
	


	//bottom left
	gl_Position = vp * vec4(g_pos, 1.0);
	g_tex_coords = vec2(1.0, 1.0);
	EmitVertex();

	//top left
	g_pos.y += 1.0;
	gl_Position = vp * vec4(g_pos, 1.0);

	g_tex_coords = vec2(1.0, 0.0);
	EmitVertex();

	//bottom right
	g_pos.y -= 1.0;
	g_pos += right;
	gl_Position = vp * vec4(g_pos, 1.0);

	g_tex_coords = vec2(0.0, 1.0);
	EmitVertex();


	//top right

	g_pos.y += 1.0;
	gl_Position = vp * vec4(g_pos, 1.0);

	g_tex_coords = vec2(0.0, 0.0);
	EmitVertex();

	EndPrimitive();



	

}
