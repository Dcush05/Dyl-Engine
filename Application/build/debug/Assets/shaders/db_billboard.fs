#version 330 core

uniform sampler2D u_texture;
in vec2 g_tex_coords;
out vec4 FragColor;


void main()
{
	vec4 color = vec4(1.0,1.0,1.0,1.0);
	FragColor = color * texture(u_texture, g_tex_coords);
	if(FragColor.r == 0 && FragColor.g == 0 && FragColor.b == 0)
	{
		discard;
	}

}
