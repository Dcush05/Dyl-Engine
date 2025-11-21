#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>
out vec2 TexCoords; 
uniform mat4 model;
uniform mat4 projection; 
// Texture coordinates for the region to render
uniform vec4 textureRegion; // <x, y, width, height> in texture space (0.0 to 1.0)
void main()
{
    // Map the full texture coordinates (0.0-1.0) to the specified region
    // vertex.zw contains the original texture coordinates
    vec2 regionCoords = textureRegion.xy + vertex.zw * textureRegion.zw;
    TexCoords = regionCoords; 
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}
