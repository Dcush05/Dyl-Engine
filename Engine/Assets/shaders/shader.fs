#version 330 core
uniform bool useTexture;
in vec2 TexCoords;
out vec4 color;
uniform sampler2D image;
uniform vec4 aColor;

void main()
{    
	color = aColor * texture(image, TexCoords);
	 
}  
