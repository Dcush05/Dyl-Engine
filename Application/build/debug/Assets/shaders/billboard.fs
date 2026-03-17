#version 330 core

uniform sampler2D u_texture;
in vec2 vtex_coords
out vec4 FragColor;


void main()
{
	FragColor = texture(u_texture, vtex_coords);
	if(FragColor.r == 0 && FragColor.g == 0 && FragColor.b == 0)
	{
		discard;
	}

}
