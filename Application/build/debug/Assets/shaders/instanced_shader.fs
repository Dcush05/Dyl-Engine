#version 330 core

uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D ambient;
uniform sampler2D specular_highlight;
uniform sampler2D alpha;
uniform sampler2D displacement;
//uniform sampler2D bump;

in vec2 o_tex_coords;
out vec4 FragColor;
uniform bool has_texture;
vec3 model_color;


//MAKE THIS BETTER, CORRESPONDING TO THE ACTUAL TEXTURES NORMALS
void main()
{
	//FragColor = vec4(color, 1.0);
	vec3 model_color = vec3(1.0,1.0,1.0);
	if(has_texture)
	{


		vec3 diffuseColor = texture(diffuse, o_tex_coords).rgb;
		vec3 specularColor = texture(specular, o_tex_coords).rgb;

		//hardcoded values for now will change this in the future
		vec3 ambient = 0.1 * diffuseColor;
		vec3 diffuseLight = 0.1 * diffuseColor;
		vec3 specularLight = 0.1 * specularColor;


		model_color = ambient + diffuseLight + specularLight;
	
	}

	FragColor = vec4(model_color, 1.0);
	
}
