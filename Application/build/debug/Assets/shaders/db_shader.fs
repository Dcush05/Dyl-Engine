#version 330 core

in vec4 vertexColor; 
out vec4 FragColor;
in vec2 a_tex_coords;
uniform sampler2D u_texture;
uniform bool need_texture;
uniform bool is_sky_box;
uniform samplerCube cubemap;
uniform vec4 light_color;
uniform bool is_light;

uniform float ambient_strength;
uniform float specular_strength;


//START OF LIGHT TYPE ENUM
const int LIGHTING_NONE = 0;
const int LIGHTING_AMBIENT = 1;
const int LIGHTING_DIFFUSE = 2;
const int LIGHTING_SPECULAR = 3;
//END OF ENUM

uniform int light_type;
in vec3 a_texture_dir;

in vec3 frag_pos;
in vec3 frag_normals;
uniform vec3 light_pos;

uniform vec3 view_pos;

void main()
{

	vec3 norm = normalize(frag_normals);
	vec3 light_dir = normalize(light_pos - frag_pos);

	vec3 view_dir = normalize(view_pos - frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);


	vec3 ambient = ambient_strength * light_color.rgb;	

	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_color.rgb;

	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_strength * spec * light_color.rgb;

	vec3 result = vec3(1.0,1.0,1.0);
	if(light_type == LIGHTING_AMBIENT)
	{
		result = ambient;
	}else if(light_type == LIGHTING_DIFFUSE)
	{
		result = (ambient + diffuse);
	}else if(light_type == LIGHTING_SPECULAR)
	{
		result = (ambient + diffuse + specular);
	}
	if(need_texture) {

		FragColor = (vertexColor * vec4(result, 1.0)) * texture(u_texture, a_tex_coords);
	}else if(is_sky_box){
		FragColor = vertexColor * texture(cubemap, a_texture_dir);
	}else{
		FragColor = vertexColor * vec4(result, 1.0);
	}

	if(is_light)
	{
		FragColor = vertexColor;
	}
}

