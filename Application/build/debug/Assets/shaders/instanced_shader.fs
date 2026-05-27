#version 330 core

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D ambient;
uniform sampler2D specular_highlight;
uniform sampler2D alpha;
uniform sampler2D displacement;
uniform sampler2D bump;

uniform vec3 light_pos;
uniform float diffuse_strength;
uniform float specular_strength;


uniform vec3 camera_pos;

in vec2 o_tex_coords;
in vec3 o_normals;
in vec3 o_pos;
in vec4 o_color;


out vec4 FragColor;
uniform bool has_texture;
vec3 model_color;


//MAKE THIS BETTER, CORRESPONDING TO THE ACTUAL TEXTURES NORMALS
void main()
{
	//FragColor = vec4(color, 1.0);
	vec3 model_color = o_color.rgb;
	if(has_texture)
	{
		vec3 N = normalize(o_normals);
		vec3 normal_tex = texture(bump, o_tex_coords).rgb;
		N = normalize(N + normal_tex * 0.5);

		vec3 L = normalize(light_pos - o_pos);
		vec3 V = normalize(camera_pos - o_pos);
		vec3 R = reflect(-L, N);

		vec3 diffuseColor = texture(diffuse, o_tex_coords).rgb;
		vec3 specularColor = texture(specular, o_tex_coords).rgb;


		float final_diff = max(dot(N, L), 0.0);
		float final_spec = pow(max(dot(V, R), 0.0), 32.0);

		//hardcoded values for now will change this in the future
		//you can send ambient strength as well
		vec3 ambient = 0.1 * diffuseColor;
		vec3 diffuse_light = final_diff * diffuseColor;
		vec3 specular_light = final_spec * specularColor;
		model_color = ambient + diffuse_light + specular_light;
	//	model_color = normal_tex;
	}
	//NOTE: We must be able to handle lighting as well when there is no textures involved

	FragColor = vec4(model_color, 1.0);
	
}
