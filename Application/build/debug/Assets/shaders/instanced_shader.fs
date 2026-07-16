#version 450 core
#extension GL_ARB_bindless_texture : require

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

uniform int texture_index;
uniform int diffuse_idx;
uniform int specular_idx;
uniform int ambient_idx;
uniform int bump_idx;
uniform int alpha_idx;
uniform int displacement_idx;
uniform int specular_highlight_idx;


uniform bool is_bindless;


layout(binding = 0, std430) readonly buffer sssbo1
{
	sampler2D textures[];
};

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
	//FragColor = vec4(0.5, 0.5, 0.5, 1.0);
	vec3 model_color = o_color.rgb;
	if(has_texture)
	{
		if(!is_bindless)
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
		

		}else{
			vec3 N = normalize(o_normals);
			if(bump_idx >= 0)
			{

				vec3 normal_tex = texture(textures[bump_idx], o_tex_coords).rgb;
				N = normalize(N + normal_tex * 0.5);
			}



			//sampler2D specular_tex = textures[texture_index];
			//sampler2D ambient_tex = textures[texture_index];





			vec3 L = normalize(light_pos - o_pos);
			vec3 V = normalize(camera_pos - o_pos);
			vec3 R = reflect(-L, N);

			vec3 diffuseColor = (diffuse_idx >= 0) ? texture(textures[diffuse_idx], o_tex_coords).rgb : vec3(1.0);
			vec3 specularColor = (specular_idx >= 0) ? texture(textures[specular_idx], o_tex_coords).rgb : vec3(0.0);


			float final_diff = max(dot(N, L), 0.0);
			float final_spec = pow(max(dot(V, R), 0.0), 32.0);

			//hardcoded values for now will change this in the future
			//you can send ambient strength as well
			vec3 ambient = 0.1 * diffuseColor;
			vec3 diffuse_light = final_diff * diffuseColor;
			vec3 specular_light = final_spec * specularColor;
			model_color = ambient + diffuse_light + specular_light;



		}
	}
	//NOTE: We must be able to handle lighting as well when there is no textures involved


	FragColor = vec4(model_color, 1.0);
	
}
