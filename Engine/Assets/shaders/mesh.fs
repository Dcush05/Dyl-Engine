#version 330 core

in vec2 TexCoords;

out vec4 FragColor;
in vec3 Color;

//uniform sampler2D image;

void main()
{
	//vec4 tex = texture(image, TexCoords);
//	FragColor = texture(image, TexCoords);
	
	FragColor = vec4(Color, 1.0);
}
