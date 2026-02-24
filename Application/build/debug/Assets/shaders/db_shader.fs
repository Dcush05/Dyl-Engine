#version 330 core

in vec4 vertexColor; 
out vec4 FragColor;
in vec2 a_tex_coords;
uniform sampler2D u_texture;
uniform bool need_texture;
uniform bool is_sky_box;
uniform samplerCube cubemap;
in vec3 a_texture_dir;
void main()
{
	if(need_texture) {

		FragColor = vertexColor * texture(u_texture, a_tex_coords);
	}else if(is_sky_box){
		FragColor = vertexColor * texture(cubemap, a_texture_dir);
	}else{
		FragColor = vertexColor;
	}

}

