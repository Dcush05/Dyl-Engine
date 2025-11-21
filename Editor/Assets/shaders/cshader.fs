#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D image;
uniform vec4 aColor;
uniform vec4 textureRegion; // x, y, w, h
uniform bool is_texture;
void main()
{
    // Convert local TexCoords to atlas coords
    vec2 texCoord = vec2(
        textureRegion.x + TexCoords.x * textureRegion.z,
        textureRegion.y + (1.0 - TexCoords.y) * textureRegion.w
    );
    vec4 texColor = texture(image, texCoord);
    
    // Use aColor as tint — you can remove it if not needed
	if(is_texture)
	{
		FragColor = aColor * texColor;
	} else 
	{
		FragColor = aColor;
	}
}

