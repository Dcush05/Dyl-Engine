#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec4 color;
layout (location = 3) in vec3 tex_coords;
layout (location = 4) in float rotation;
layout (location = 5) in vec3 size;

uniform mat4 projection;
uniform mat4 view;
uniform bool is_3d;

uniform bool is_ui;

out vec4 vertexColor;
out vec2 a_tex_coords;
out vec3 a_texture_dir;
out vec3 frag_normals;
out vec3 frag_pos;
out vec2 frag_ui_pos;
out vec2 frag_uv;
out vec3 frag_size;

void main()
{

/*	float cos_r = cos(rotation);	
	float sin_r = sin(rotation);

	vec2 half_size = size.xy * 0.5f;

	float rotated_x = position.x * cos_r - position.y * sin_r;
	float rotated_y = position.x * sin_r + position.y * cos_r;
	vec3 new_pos;
	new_pos.x = rotated_x;
	new_pos.y = rotated_y;*/

	if(!is_3d)
	{
		vec2 center_offset = (tex_coords.xy - 0.5) * size.xy;
		vec2 center = position.xy - center_offset;

		vec2 to_vertex = position.xy - center;

		float cos_r = cos(rotation);
		float sin_r = sin(rotation);

		float rotated_x = to_vertex.x * cos_r - to_vertex.y * sin_r;
		float rotated_y = to_vertex.x * sin_r + to_vertex.y * cos_r;

		vec2 final_pos = vec2(rotated_x, rotated_y) + center;	
		if(!is_ui)
		{

			gl_Position = projection * view * vec4(position, 1.0);
		}else{


			gl_Position = projection * vec4(position, 1.0);

		}

	}else{
		gl_Position = projection * view * vec4(position, 1.0);
	}

	//NOTE: a prefix is for attributes when refactoring change it
	vertexColor = color;
	a_tex_coords = tex_coords.xy;
	a_texture_dir = position;
	frag_normals = normals;
	frag_pos = position;
	frag_ui_pos = (tex_coords.xy * vec2(0.5)) * size.xy;
	frag_size = size;
	frag_uv = a_tex_coords.xy;
}
