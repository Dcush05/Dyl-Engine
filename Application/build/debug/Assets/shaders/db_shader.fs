#version 330 core

in vec4 vertexColor; 
out vec4 FragColor;
in vec2 a_tex_coords;
uniform sampler2D u_texture;
uniform bool need_texture;
void main()
{
	if(need_texture) {

		FragColor = vertexColor * texture(u_texture, a_tex_coords);
	}else{
		FragColor = vertexColor;
	}

}

